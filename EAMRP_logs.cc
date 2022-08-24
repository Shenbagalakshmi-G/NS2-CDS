#ifndef __EAMRP_h__
#define __EAMRP_h__

#include <agent.h>
#include <packet.h>
#include <sys/types.h>
#include <cmu/list.h>
#include <scheduler.h>

#include <cmu-trace.h>
#include <priqueue.h>
#include <EAMRP/EAMRP_rtable.h>
#include <EAMRP/EAMRP_rqueue.h>
#include <classifier/classifier-port.h>
#include <smac.h>
 
#define EAMRP_LOCAL_REPAIR
#define EAMRP_LINK_LAYER_DETECTION
#define EAMRP_USE_LL_METRIC

class EAMRP;

#define MY_ROUTE_TIMEOUT        10                      	
#define ACTIVE_ROUTE_TIMEOUT    10				
#define REV_ROUTE_LIFE          6				
#define BCAST_ID_SAVE           6				
#define RREQ_RETRIES            3  
#define MAX_RREQ_TIMEOUT	10.0 
#define TTL_START     5
#define TTL_THRESHOLD 7
#define TTL_INCREMENT 2 
#define NODE_TRAVERSAL_TIME     0.03             
#define LOCAL_REPAIR_WAIT_TIME  0.15 
#define NETWORK_DIAMETER        30             
#define RREP_WAIT_TIME         1.0  
#define ID_NOT_FOUND    0x00
#define ID_FOUND        0x01
#define DELAY 1.0          
#define NO_DELAY -1.0       
#define ARP_DELAY 0.01     
#define HELLO_INTERVAL          1               
#define ALLOWED_HELLO_LOSS      3               
#define BAD_LINK_LIFETIME       3               
#define MaxHelloInterval        (1.25 * HELLO_INTERVAL)
#define MinHelloInterval        (0.75 * HELLO_INTERVAL)

class BroadcastTimer : public Handler 
{
   public:
        BroadcastTimer(EAMRP* a) : agent(a) {}
        void handle(Event*);
   private:
        EAMRP   *agent;
	Event	intr;
};

class HelloTimer : public Handler 
{
   public:
        HelloTimer(EAMRP* a) : agent(a) {}
        void handle(Event*);
   private:
        EAMRP   *agent;
	Event	intr;
};

class NeighborTimer : public Handler 
{
   public:
        NeighborTimer(EAMRP* a) : agent(a) {}
        void handle(Event*);
   private:
        EAMRP   *agent;
	Event	intr;
};

class RouteCacheTimer : public Handler 
{
   public:
        RouteCacheTimer(EAMRP* a) : agent(a) {}
        void handle(Event*);
   private:
        EAMRP   *agent;
	Event	intr;
};

class LocalRepairTimer : public Handler 
{
   public:
        LocalRepairTimer(EAMRP* a) : agent(a) {}
        void handle(Event*);
   private:
        EAMRP   *agent;
	Event	intr;
};

class BroadcastID 
{
   friend class EAMRP;
   public:
        BroadcastID(nsaddr_t i, u_int32_t b) { src = i; id = b;  }
   protected:
        LIST_ENTRY(BroadcastID) link;
        nsaddr_t        src;
        u_int32_t       id;
        double          expire;        
};

LIST_HEAD(EAMRP_bcache, BroadcastID);

class EAMRP: public Agent 
{
   friend class EAMRP_rt_entry;
   friend class BroadcastTimer;
   friend class HelloTimer;
   friend class NeighborTimer;
   friend class RouteCacheTimer;
   EAMRP(nsaddr_t id);
   SMAC *mymac;
   void	recv(Packet *p, Handler *);
  
   protected:
      int command(int, const char *const *);
      int initialized() { return 1 && target_; }
      void rt_resolve(Packet *p);
      void rt_update(EAMRP_rt_entry *rt, u_int32_t seqnum,u_int16_t metric, nsaddr_t nexthop,double expire_time, int index);
      void rt_down(EAMRP_rt_entry *rt);
      void local_rt_repair(EAMRP_rt_entry *rt, Packet *p);
   public:
      void rt_ll_failed(Packet *p);
      void handle_link_failure(nsaddr_t id);
   protected:
      void rt_purge(void);
      void enque(EAMRP_rt_entry *rt, Packet *p);
      Packet* deque(EAMRP_rt_entry *rt);
      void nb_insert(nsaddr_t id, double dEnergy, double dLoss, double dBW);
      void nb_update(nsaddr_t id, double dEnergy, double dLoss, double dBW);
      EAMRP_Neighbor* nb_lookup(nsaddr_t id);
      void nb_delete(nsaddr_t id);
      void nb_purge(void);
      void id_insert(nsaddr_t id, u_int32_t bid);
      bool id_lookup(nsaddr_t id, u_int32_t bid);
      void id_purge(void);
      void forward(EAMRP_rt_entry *rt, Packet *p, double delay);
      void sendHello(void);
      void sendRequest(nsaddr_t dst);
      void sendReply(nsaddr_t ipdst, u_int32_t hop_count,nsaddr_t rpdst, u_int32_t rpseq,u_int32_t lifetime, double timestamp);
      void sendError(Packet *p, bool jitter = true);
      void recvEAMRP(Packet *p);
      void recvHello(Packet *p);
      void recvRequest(Packet *p);
      void recvReply(Packet *p);
      void recvError(Packet *p);
      double PerHopTime(EAMRP_rt_entry *rt);
      nsaddr_t index;                  
      u_int32_t seqno;                  
      int bid;                    
      EAMRP_rtable rthead;                 
      EAMRP_ncache nbhead;                
      EAMRP_bcache bihead;                
      BroadcastTimer  btimer;
      HelloTimer      htimer;
      NeighborTimer   ntimer;
      RouteCacheTimer rtimer;
      LocalRepairTimer lrtimer;
      EAMRP_rtable rtable;
      EAMRP_rqueue rqueue;
      Trace *logtarget;
      PriQueue *ifqueue;
      void log_link_del(nsaddr_t dst);
      void log_link_broke(Packet *p);
      void log_link_kept(nsaddr_t dst);
      PortClassifier *dmux_;
};

#endif
