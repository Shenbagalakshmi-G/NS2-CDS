#ifndef __EAMRP_packet_h__
#define __EAMRP_packet_h__
#define EAMRP_MAX_ERRORS 100
#define EAMRPTYPE_HELLO  	0x01
#define EAMRPTYPE_RREQ   	0x02
#define EAMRPTYPE_RREP   	0x04
#define EAMRPTYPE_RERR   	0x08
#define EAMRPTYPE_RREP_ACK  	0x10
#define HDR_EAMRP(p)		((struct hdr_EAMRP*)hdr_EAMRP::access(p))
#define HDR_EAMRP_REQUEST(p)  	((struct hdr_EAMRP_request*)hdr_EAMRP::access(p))
#define HDR_EAMRP_REPLY(p)	((struct hdr_EAMRP_reply*)hdr_EAMRP::access(p))
#define HDR_EAMRP_ERROR(p)	((struct hdr_EAMRP_error*)hdr_EAMRP::access(p))
#define HDR_EAMRP_RREP_ACK(p)	((struct hdr_EAMRP_rrep_ack*)hdr_EAMRP::access(p))

struct hdr_EAMRP 
{
   u_int8_t ah_type;
   static int offset_; 
   inline static int& offset() 
   { 
      return offset_; 
   }
   inline static hdr_EAMRP* access(const Packet* p) 
   {
      return (hdr_EAMRP*) p->access(offset_);
   }
};

struct hdr_EAMRP_request 
{
   u_int8_t        rq_type;	
   u_int8_t        reserved[2];
   u_int8_t        rq_hop_count;   
   u_int32_t       rq_bcast_id;    
   nsaddr_t        rq_dst;         
   u_int32_t       rq_dst_seqno;   
   nsaddr_t        rq_src;        
   u_int32_t       rq_src_seqno;   
   double          rq_timestamp;   
   nsaddr_t        rq_first_hop;  

   inline int size() 
   { 
      int sz = 0;
      sz = 7*sizeof(u_int32_t);
      sz += sizeof(nsaddr_t);    
      assert (sz >= 0);
      return sz;
   }
};

struct hdr_EAMRP_reply 
{
   u_int8_t        rp_type;        
   u_int8_t        reserved[2];
   u_int8_t        rp_hop_count;           
   nsaddr_t        rp_dst;                 
   u_int32_t       rp_dst_seqno;           
   nsaddr_t        rp_src;                 
   double	   rp_lifetime;            
   double          rp_timestamp;          
   u_int32_t       rp_bcast_id;           
   nsaddr_t        rp_first_hop;
						
   inline int size() 
   { 
      int sz = 0;
      sz = 6*sizeof(u_int32_t);
      if (rp_type == EAMRPTYPE_RREP) 
      {
         sz += sizeof(u_int32_t);   
         sz += sizeof(nsaddr_t);    
      }
      assert (sz >= 0);
      return sz;
  }

};

struct hdr_EAMRP_error 
{
   u_int8_t        re_type;               
   u_int8_t        reserved[2];            
   u_int8_t        DestCount;                
   nsaddr_t        unreachable_dst[EAMRP_MAX_ERRORS];   
   u_int32_t       unreachable_dst_seqno[EAMRP_MAX_ERRORS];   

   inline int size() 
   { 
      int sz = 0;
      sz = (DestCount*2 + 1)*sizeof(u_int32_t);
      assert(sz);
      return sz;
   }
};

struct hdr_EAMRP_rrep_ack 
{
   u_int8_t	rpack_type;
   u_int8_t	reserved;
};
 
union hdr_all_EAMRP 
{
  hdr_EAMRP          ah;
  hdr_EAMRP_request  rreq;
  hdr_EAMRP_reply    rrep;
  hdr_EAMRP_error    rerr;
  hdr_EAMRP_rrep_ack rrep_ack;
};

#endif 
