#ifndef __EAMRP_rtable_h__
#define __EAMRP_rtable_h__
#include <assert.h>
#include <sys/types.h>
#include <config.h>
#include <lib/bsd-list.h>
#include <scheduler.h>
#define CURRENT_TIME Scheduler::instance().clock()
#define INFINITY2 0xff

class EAMRP_Neighbor 
{
   friend class EAMRP;
   friend class EAMRP_rt_entry;
   public:
      EAMRP_Neighbor(u_int32_t a) { nb_addr = a; }
   protected:
      LIST_ENTRY(EAMRP_Neighbor) nb_link;
      nsaddr_t        nb_addr;
      double          nb_expire;      
};

LIST_HEAD(EAMRP_ncache, EAMRP_Neighbor);
 
class EAMRP_Path 
{
   friend class EAMRP;
   friend class EAMRP_rt_entry;
   public:
      EAMRP_Path(nsaddr_t nh, u_int16_t h, double expire_time, nsaddr_t lh=0) 
      { 
         nexthop = nh;
         hopcount = h;
         expire = expire_time;
         ts = Scheduler::instance().clock();
         lasthop = lh;
         error = false;
      }
      void printPath() 
      { 
         printf("                        %6d  %6d  %6d\n", nexthop, hopcount, lasthop);
      }
      void printPaths() 
      {
         EAMRP_Path *p = this;
	 for (; p; p = p->path_link.le_next) 
         {
	    p->printPath();
	 }
      }
      protected:
         LIST_ENTRY(EAMRP_Path) path_link;
         nsaddr_t nexthop;    
         u_int16_t hopcount;   
         double expire;     
         double ts;        
         nsaddr_t lasthop;   
         bool error;
};

BEGIN 
{
   send = 0;
   recv = 0;
   bytes = 0;
   st = 0;
   ft = 0;
   rtr = 0;
   delay = 0;
   aodv_ = 0;
   last_pkt_recv=0;
   jitter=0;
   j_count=0;
   nodes=0;
}
{
if ( $16 == "-Ne" )
{
   if (initial_energy[$9] == "")
   initial_energy[$9] = $17;
   energy[$9] = $17;
   if(nodes < $9)
   nodes = $9;
}

if ( $19 == "RTR" && $1 == "s" && $35 != "cbr" && $35 != "udp" && $35 != "ack")
rtr++;

if ( $19 == "AGT" && $1 == "s")
{
   if(send == 0)
   st = $3;
   ft = $3;
   st_time[$41] = $3;
   send++;
}

if ( $19 == "AGT" && $1 == "r")
{
if(recv == 0)
{
   last_pkt_recv = $3;
}
else
{
   jitter+=$3 - last_pkt_recv;
   j_count++;
   last_pkt_recv	=	$3
}
   recv++;
   bytes+=$37
   ft_time[$41] = $3;
   delay += ft_time[$41]-st_time[$41]
}
}

END 
{
   total_energy = 0;
   residual=0;
   for (i=0;i<nodes;i++) 
{
   residual_energy[i] = initial_energy[i] - energy[i];
   total_energy+=residual_energy[i];
   residual+=energy[i];
}
}

LIST_HEAD(EAMRP_paths, EAMRP_Path);
class EAMRP_Precursor 
{
   friend class EAMRP;
   friend class EAMRP_rt_entry;
   public:
      EAMRP_Precursor(u_int32_t a) { pc_addr = a; }
   protected:
      LIST_ENTRY(EAMRP_Precursor) pc_link;
      nsaddr_t        pc_addr;	
};

LIST_HEAD(EAMRP_precursors, EAMRP_Precursor);
class EAMRP_rt_entry 
{
   friend class EAMRP_rtable;
   friend class EAMRP;
   friend class LocalRepairTimer;
   public:
      EAMRP_rt_entry();
      ~EAMRP_rt_entry();
      void            nb_insert(nsaddr_t id);
      EAMRP_Neighbor*  nb_lookup(nsaddr_t id);
      EAMRP_Path*   path_insert(nsaddr_t nexthop, u_int16_t hopcount, double expire_time, nsaddr_t lasthop=0);
      EAMRP_Path*   path_lookup(nsaddr_t id);  
      EAMRP_Path*   disjoint_path_lookup(nsaddr_t nexthop, nsaddr_t lasthop);
      bool         new_disjoint_path(nsaddr_t nexthop, nsaddr_t lasthop);
      EAMRP_Path*   path_lookup_lasthop(nsaddr_t id);   
      void         path_delete(nsaddr_t id);           
      void         path_delete(void);                 
      void         path_delete_longest(void);         
      bool         path_empty(void);                   
      EAMRP_Path*   path_find(void);                   
      EAMRP_Path*   path_findMinHop(void);              
      u_int16_t    path_get_max_hopcount(void);  
      u_int16_t    path_get_min_hopcount(void);  
      double       path_get_max_expiration_time(void); 
      void         path_purge(void);
      void            pc_insert(nsaddr_t id);
      EAMRP_Precursor* pc_lookup(nsaddr_t id);
      void 		pc_delete(nsaddr_t id);
      void 		pc_delete(void);
      bool 		pc_empty(void);
      double          rt_req_timeout;        
      u_int8_t        rt_req_cnt;             
      u_int8_t        rt_flags;
   protected:
        LIST_ENTRY(EAMRP_rt_entry) rt_link;

        nsaddr_t        rt_dst;
        u_int32_t       rt_seqno;
 
        u_int16_t       rt_hops;            
        u_int16_t       rt_advertised_hops; 
	int 		rt_last_hop_count;	
 
        EAMRP_paths      rt_path_list;    
        u_int32_t       rt_highest_seqno_heard; 
        int             rt_num_paths_;
	bool rt_error;
        
        EAMRP_precursors rt_pclist;
        double          rt_expire;     		

#define RTF_DOWN 0
#define RTF_UP 1
#define RTF_IN_REPAIR 2
 

#define MAX_HISTORY	3
	double 		rt_disc_latency[MAX_HISTORY];
	char 		hist_indx;
        int 		rt_req_last_ttl;        
	 
        EAMRP_ncache          rt_nblist;
};


 

class EAMRP_rtable {
 public:
	EAMRP_rtable() { LIST_INIT(&rthead); }

        EAMRP_rt_entry*       head() { return rthead.lh_first; }

        EAMRP_rt_entry*       rt_add(nsaddr_t id);
        void                 rt_delete(nsaddr_t id);
        EAMRP_rt_entry*       rt_lookup(nsaddr_t id);
 
	void                 rt_dumptable();
	bool                 rt_has_active_route();

 private:
        LIST_HEAD(EAMRP_rthead, EAMRP_rt_entry) rthead;
};

#endif
