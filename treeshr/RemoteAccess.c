#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ncidef.h>
#include <string.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define MAX_DIMS 7
struct descrip { char dtype;
                 char ndims;
                 int  dims[MAX_DIMS];
                 int  length;
                 void *ptr;
               };

#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

extern char *TranslateLogical(char *);
extern void TranslateLogicalFree(char *);
extern int LibFindImageSymbol();

static int FindImageSymbol(char *name, void **sym)
{
  static DESCRIPTOR(image,"MdsIpShr");
  struct descriptor symname = {0, DTYPE_T, CLASS_S, 0};
  symname.length = strlen(name);
  symname.pointer = name;
  return LibFindImageSymbol(&image,&symname,sym);
}

static struct _host_list {char *host; int socket; int connections; struct _host_list *next;} *host_list = 0;

static int ConnectToMds(char *host)
{
  struct _host_list *hostchk;
  struct _host_list **nextone;
  static int (*rtn)(char *) = 0;
  int socket;
  if (rtn == 0)
  {
    int status = FindImageSymbol("ConnectToMds",(void **)&rtn);
    if (!(status & 1)) return -1;
  }
  for (nextone = &host_list,hostchk = host_list; hostchk; nextone = &hostchk->next, hostchk = hostchk->next)
  {
    if (strcmp(hostchk->host,host) == 0)
    {
      hostchk->connections++;
      return hostchk->socket;
    }
  }
  socket = (*rtn)(host);
  if (socket != -1)
  {
    *nextone = malloc(sizeof(struct _host_list));
    (*nextone)->host = strcpy(malloc(strlen(host)+1),host);
    (*nextone)->socket = socket;
    (*nextone)->connections = 1;
    (*nextone)->next = 0;
  }
  return socket;
} 
  
static int DisconnectFromMds(int socket)
{
  int status = 1;
  struct _host_list *hostchk;
  struct _host_list *previous;
  static int (*rtn)(int) = 0;
  if (rtn == 0)
  {
    int status = FindImageSymbol("DisconnectFromMds",(void **)&rtn);
    if (!(status & 1)) return status;
  }
  for (previous = 0,hostchk = host_list; hostchk && hostchk->socket != socket; previous=hostchk, 
                                                                               hostchk = hostchk->next);
  if (hostchk)
  {
    hostchk->connections--;
    if (hostchk->connections <= 0)
    {
      status = (*rtn)(socket);
      free(hostchk->host);
      if (previous)
        previous->next = hostchk->next;
      else
        host_list = hostchk->next;
      free(hostchk);
    }
  }
  return status;
} 
  
static int (*MdsValue)() = 0;
  
static int MdsValue0(int socket, char *exp, struct descrip *ans)
{
  if (MdsValue == 0)
  {
    int status = FindImageSymbol("MdsValue",(void **)&MdsValue);
    if (!(status & 1)) return status;
  }
  return (*MdsValue)(socket, exp, ans, 0);
} 
  
int ConnectTreeRemote(PINO_DATABASE *dblist, char *tree, char *subtree_list,int status)
{
  int len = strlen(tree);
  char tree_lower[13];
  char pathname[32];
  int i;
  char *resnam = 0;
  char *logname;
  char *colon = 0;
  for (i=0;i<len && i < 12;i++)
     tree_lower[i] = __tolower(tree[i]);
  tree_lower[i]=0;
  strcpy(pathname,tree_lower);
  strcat(pathname,TREE_PATH_SUFFIX);
  logname = TranslateLogical(pathname);
  if (logname)
  {
    if ((colon = strchr(logname, ':')) != 0 && colon[1] == ':')
    {
      int socket;
      *colon = '\0';
      socket = ConnectToMds(logname);
      if (socket != -1)
      {
        struct descrip ans;
        char *exp = malloc(strlen(subtree_list ? subtree_list : tree)+100);
        sprintf(exp,"TreeOpen('%s',%d)",subtree_list ? subtree_list : tree,dblist->shotid);
        status =  MdsValue0(socket, exp, &ans);
        if (status & 1)
	{
          TREE_INFO *info;
          /***********************************************
           Get virtual memory for the tree
           information structure and zero the structure.
          ***********************************************/
          for (info = dblist->tree_info; info && strcmp(tree,info->treenam); info = info->next_info);
          if (!info)
	  {
            info = malloc(sizeof(TREE_INFO));
            if (info)
            {
              static TREE_HEADER header;
  	      memset(info,0,sizeof(*info));
              info->blockid = TreeBLOCKID;
	      info->flush = (dblist->shotid == -1);
              info->header = &header;
	      info->treenam = strcpy(malloc(strlen(tree)+1),tree);
	      TreeCallHook(OpenTree, info);
              info->channel = socket;
	      dblist->tree_info = info;
              dblist->remote = 1;
            }
	  }
        }
      }
    }
    TranslateLogicalFree(logname);
  }
  return status;
}

int SetStackSizeRemote(PINO_DATABASE *dblist, int stack_size)
{
	return 1;
}

int CloseTreeRemote(PINO_DATABASE *dblist, int call_hook)
{
	return 1;
}

int GetRecordRemote(PINO_DATABASE *dblist, int nid_in, struct descriptor_xd *dsc)
{
  struct descrip ans;
  int status;
  char exp[512];
  sprintf(exp,"SerializeOut(`getnci(%d,'RECORD'))",nid_in);
  status = MdsValue0(dblist->tree_info->channel,exp,&ans);
  if (status & 1)
  {
    if (ans.ptr)
      status = MdsSerializeDscIn(ans.ptr,dsc);
    else
      status = TreeNODATA;
  }
  return status;
}

int FindNodeRemote(PINO_DATABASE *dblist, char *path, int *outnid)
{
  struct descrip ans;
  int status;
  char *exp = malloc(strlen(path)+32);
  sprintf(exp,"getnci(%s,'nid_number')",path);
  status = MdsValue0(dblist->tree_info->channel,exp,&ans);
  if (status & 1)
  {
    if (ans.ptr)
      *outnid = *(int *)ans.ptr;
    else
      status = TreeNNF;
  }
  return status;
}

struct _FindNodeStruct { int *nids;
                         int num;
                       };
int FindNodeWildRemote(PINO_DATABASE *dblist, char *path, int *nid_out, void **ctx_inout, int usage_mask)
{
  int status = TreeNORMAL;
  struct _FindNodeStruct *ctx = (struct _FindNodeStruct *)*ctx_inout;
  if (!ctx)
  {
    struct descrip ans;
    int status = 0;
    char *exp = malloc(strlen(path)+50);
    sprintf(exp,"TreeFindNodeWild('%s',%d)",path,usage_mask);
    status = MdsValue0(dblist->tree_info->channel,exp,&ans);
    if (status & 1)
    {
      if (ans.ptr)
      {
        ctx = malloc(sizeof(struct _FindNodeStruct) + ans.dims[0] * sizeof(int));
        ctx->nids = (int *)((char *)ctx + sizeof(struct _FindNodeStruct));
        memcpy(ctx->nids,ans.ptr,ans.dims[0] * sizeof(int));
        ctx->num = ans.dims[0];
        *ctx_inout = (void *)ctx;
      }
      else
        status = TreeNNF;
    }
  }
  if (status & 1)
  {
    if (ctx->num > 0)
    {
      *nid_out = *ctx->nids;
      ctx->num--;
      ctx->nids++;
    }
    else
    {
      free(ctx);
      *ctx_inout = 0;
      status = TreeNMN;
    }
  }
  return status;
}

int FindNodeEndRemote(PINO_DATABASE *dblist, void **ctx_in)
{
  if (*ctx_in)
  {
    free(*ctx_in);
    *ctx_in = 0;
  }
  return 1;
}

char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in, void **ctx_ptr)
{
	return NULL;
}

char *AbsPathRemote(PINO_DATABASE *dblist, char *inpath)
{
	return NULL;
}

int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid)
{
	return 1;
}

int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid)
{
	return 1;
}
int GetNciRemote(PINO_DATABASE *dblist, int nid_in, struct nci_itm *nci_itm)
{
  NID nid = *(NID *)&nid_in;
  int       status = TreeNORMAL;
  NCI_ITM  *itm;
  struct descrip ans;
  for (itm = nci_itm; itm->code != NciEND_OF_LIST && status & 1; itm++)
  {
    char    *getnci_str = NULL;
    char *string = NULL;
    switch (itm->code)
    {
    case NciDEPTH:               getnci_str = "getnci(%d,'depth')"; break;
    case NciGET_FLAGS:           getnci_str = "getnci(%d,'get_flags')"; break;
    case NciTIME_INSERTED:       getnci_str = "getnci(%d,'time_inserted')"; break;
    case NciOWNER_ID:            getnci_str = "getnci(%d,'owner')"; break;
    case NciCLASS:               getnci_str = "getnci(%d,'class')"; break;
    case NciDTYPE:               getnci_str = "getnci(%d,'dtype')"; break;
    case NciLENGTH:              getnci_str = "getnci(%d,'length')"; break;
    case NciRLENGTH:             getnci_str = "getnci(%d,'rlength')"; break;
    case NciSTATUS:              getnci_str = "getnci(%d,'status')"; break;
    case NciDATA_IN_NCI:         getnci_str = "getnci(%d,'DATA_IN_NCI')"; break;
    case NciERROR_ON_PUT:        getnci_str = "getnci(%d,'error_on_put')"; break;
    case NciIO_STATUS:           getnci_str = "getnci(%d,'io_status')"; break;
    case NciIO_STV:              getnci_str = "getnci(%d,'io_stv')"; break;
    case NciRFA:                 getnci_str = "getnci(%d,'rfa')"; break;
    case NciCONGLOMERATE_ELT:    getnci_str = "getnci(%d,'conglomerate_elt')"; break;
    case NciPARENT:              getnci_str = "getnci(getnci(%d,'parent'),'nid_number')"; break;
    case NciBROTHER:             getnci_str = "getnci(getnci(%d,'brother'),'nid_number')"; break;
    case NciMEMBER:              getnci_str = "getnci(getnci(%d,'member'),'nid_number')"; break;
    case NciCHILD:               getnci_str = "getnci(getnci(%d,'child'),'nid_number')"; break;
    case NciPARENT_RELATIONSHIP: getnci_str = "getnci(%d,'parent_relationship')"; break;
    case NciCONGLOMERATE_NIDS:   getnci_str = "getnci(%d,'conglomerate_nids')"; break;
    case NciNUMBER_OF_CHILDREN:  getnci_str = "getnci(%d,'number_of_children')"; break;
    case NciNUMBER_OF_MEMBERS:   getnci_str = "getnci(%d,'number_of_members')"; break;
    case NciNUMBER_OF_ELTS:      getnci_str = "getnci(%d,'number_of_elts')"; break;
    case NciCHILDREN_NIDS:       getnci_str = "getnci(%d,'children_nids')"; break;
    case NciMEMBER_NIDS:         getnci_str = "getnci(%d,'member_nids')"; break;
    case NciUSAGE:               getnci_str = "getnci(%d,'usage')"; break;
    case NciNODE_NAME:           getnci_str = "getnci(%d,'NODE_NAME')"; break;
    case NciPATH:                getnci_str = "getnci(%d,'path')"; break;
    case NciORIGINAL_PART_NAME:  getnci_str = "getnci(%d,'original_part_name')"; break;
    case NciFULLPATH:            getnci_str = "getnci(%d,'fullpath')"; break;
    case NciMINPATH:             getnci_str = "getnci(%d,'minpath')"; break;
    case NciPARENT_TREE:         getnci_str = "getnci(%d,'parent_tree')"; break;
    default:  		         status = TreeILLEGAL_ITEM; break;
    }
    if (status & 1)
    {
      char exp[1024];
      sprintf(exp,getnci_str,nid_in);
      status = MdsValue0(dblist->tree_info->channel,exp,&ans);
      if (status & 1)
      {
	if (ans.ptr && ans.length)
	{
          int length = ans.length * (ans.ndims ? ans.dims[0] : 1);
          if (itm->return_length_address) *itm->return_length_address = length;
          if ((ans.dtype == DTYPE_T) && (itm->pointer == 0))
	  {
            itm->pointer = memcpy(malloc(length+1),ans.ptr,length);
            ((char *)itm->pointer)[length] = '\0';
          }
          else
	  {
	    memcpy(itm->pointer, ans.ptr, min(itm->buffer_length,length));
            if (itm->buffer_length < length) status = TreeBUFFEROVF;
          }
        }
	else
	  status = 0;
      }
    }
  }
  return status;
}
