/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Nov 22 22:58:53 2007
 */
/* Compiler settings for C:\projects\no5\no5_3\no5_3.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __no5_3_h__
#define __no5_3_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __INo5Obj_FWD_DEFINED__
#define __INo5Obj_FWD_DEFINED__
typedef interface INo5Obj INo5Obj;
#endif 	/* __INo5Obj_FWD_DEFINED__ */


#ifndef ___INo5ObjEvents_FWD_DEFINED__
#define ___INo5ObjEvents_FWD_DEFINED__
typedef interface _INo5ObjEvents _INo5ObjEvents;
#endif 	/* ___INo5ObjEvents_FWD_DEFINED__ */


#ifndef __IChatObj_FWD_DEFINED__
#define __IChatObj_FWD_DEFINED__
typedef interface IChatObj IChatObj;
#endif 	/* __IChatObj_FWD_DEFINED__ */


#ifndef __IChatsObj_FWD_DEFINED__
#define __IChatsObj_FWD_DEFINED__
typedef interface IChatsObj IChatsObj;
#endif 	/* __IChatsObj_FWD_DEFINED__ */


#ifndef __IUsersObj_FWD_DEFINED__
#define __IUsersObj_FWD_DEFINED__
typedef interface IUsersObj IUsersObj;
#endif 	/* __IUsersObj_FWD_DEFINED__ */


#ifndef __IUserObj_FWD_DEFINED__
#define __IUserObj_FWD_DEFINED__
typedef interface IUserObj IUserObj;
#endif 	/* __IUserObj_FWD_DEFINED__ */


#ifndef __IYahooTxtObj_FWD_DEFINED__
#define __IYahooTxtObj_FWD_DEFINED__
typedef interface IYahooTxtObj IYahooTxtObj;
#endif 	/* __IYahooTxtObj_FWD_DEFINED__ */


#ifndef __No5Obj_FWD_DEFINED__
#define __No5Obj_FWD_DEFINED__

#ifdef __cplusplus
typedef class No5Obj No5Obj;
#else
typedef struct No5Obj No5Obj;
#endif /* __cplusplus */

#endif 	/* __No5Obj_FWD_DEFINED__ */


#ifndef __ChatObj_FWD_DEFINED__
#define __ChatObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChatObj ChatObj;
#else
typedef struct ChatObj ChatObj;
#endif /* __cplusplus */

#endif 	/* __ChatObj_FWD_DEFINED__ */


#ifndef __ChatsObj_FWD_DEFINED__
#define __ChatsObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChatsObj ChatsObj;
#else
typedef struct ChatsObj ChatsObj;
#endif /* __cplusplus */

#endif 	/* __ChatsObj_FWD_DEFINED__ */


#ifndef __UsersObj_FWD_DEFINED__
#define __UsersObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class UsersObj UsersObj;
#else
typedef struct UsersObj UsersObj;
#endif /* __cplusplus */

#endif 	/* __UsersObj_FWD_DEFINED__ */


#ifndef __UserObj_FWD_DEFINED__
#define __UserObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserObj UserObj;
#else
typedef struct UserObj UserObj;
#endif /* __cplusplus */

#endif 	/* __UserObj_FWD_DEFINED__ */


#ifndef __YahooTxtObj_FWD_DEFINED__
#define __YahooTxtObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class YahooTxtObj YahooTxtObj;
#else
typedef struct YahooTxtObj YahooTxtObj;
#endif /* __cplusplus */

#endif 	/* __YahooTxtObj_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __INo5Obj_INTERFACE_DEFINED__
#define __INo5Obj_INTERFACE_DEFINED__

/* interface INo5Obj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_INo5Obj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("559A4EE5-EB84-43F0-B3BE-8A2B290AC526")
    INo5Obj : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE About( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Chats( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [hidden][helpstring][id] */ HRESULT STDMETHODCALLTYPE Output( 
            /* [in] */ BSTR s) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IgnoreCount( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTimer( 
            /* [in] */ long nInterval,
            /* [retval][out] */ long __RPC_FAR *pTimer) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyTimer( 
            /* [in] */ long timer) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveView( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChatByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDisp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddMarqueeString( 
            /* [in] */ BSTR msg) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveChat( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INo5ObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INo5Obj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INo5Obj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INo5Obj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *About )( 
            INo5Obj __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Chats )( 
            INo5Obj __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        /* [hidden][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Output )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ BSTR s);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IgnoreCount )( 
            INo5Obj __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateTimer )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ long nInterval,
            /* [retval][out] */ long __RPC_FAR *pTimer);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestroyTimer )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ long timer);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveView )( 
            INo5Obj __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetChatByName )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDisp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddMarqueeString )( 
            INo5Obj __RPC_FAR * This,
            /* [in] */ BSTR msg);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveChat )( 
            INo5Obj __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } INo5ObjVtbl;

    interface INo5Obj
    {
        CONST_VTBL struct INo5ObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INo5Obj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INo5Obj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INo5Obj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INo5Obj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INo5Obj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INo5Obj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INo5Obj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INo5Obj_About(This)	\
    (This)->lpVtbl -> About(This)

#define INo5Obj_get_Chats(This,pVal)	\
    (This)->lpVtbl -> get_Chats(This,pVal)

#define INo5Obj_Output(This,s)	\
    (This)->lpVtbl -> Output(This,s)

#define INo5Obj_get_IgnoreCount(This,pVal)	\
    (This)->lpVtbl -> get_IgnoreCount(This,pVal)

#define INo5Obj_CreateTimer(This,nInterval,pTimer)	\
    (This)->lpVtbl -> CreateTimer(This,nInterval,pTimer)

#define INo5Obj_DestroyTimer(This,timer)	\
    (This)->lpVtbl -> DestroyTimer(This,timer)

#define INo5Obj_get_ActiveView(This,pVal)	\
    (This)->lpVtbl -> get_ActiveView(This,pVal)

#define INo5Obj_GetChatByName(This,name,ppDisp)	\
    (This)->lpVtbl -> GetChatByName(This,name,ppDisp)

#define INo5Obj_AddMarqueeString(This,msg)	\
    (This)->lpVtbl -> AddMarqueeString(This,msg)

#define INo5Obj_get_ActiveChat(This,pVal)	\
    (This)->lpVtbl -> get_ActiveChat(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_About_Proxy( 
    INo5Obj __RPC_FAR * This);


void __RPC_STUB INo5Obj_About_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE INo5Obj_get_Chats_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB INo5Obj_get_Chats_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_Output_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [in] */ BSTR s);


void __RPC_STUB INo5Obj_Output_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE INo5Obj_get_IgnoreCount_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB INo5Obj_get_IgnoreCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_CreateTimer_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [in] */ long nInterval,
    /* [retval][out] */ long __RPC_FAR *pTimer);


void __RPC_STUB INo5Obj_CreateTimer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_DestroyTimer_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [in] */ long timer);


void __RPC_STUB INo5Obj_DestroyTimer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE INo5Obj_get_ActiveView_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB INo5Obj_get_ActiveView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_GetChatByName_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDisp);


void __RPC_STUB INo5Obj_GetChatByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INo5Obj_AddMarqueeString_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [in] */ BSTR msg);


void __RPC_STUB INo5Obj_AddMarqueeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE INo5Obj_get_ActiveChat_Proxy( 
    INo5Obj __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB INo5Obj_get_ActiveChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INo5Obj_INTERFACE_DEFINED__ */



#ifndef __NO5Lib_LIBRARY_DEFINED__
#define __NO5Lib_LIBRARY_DEFINED__

/* library NO5Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_NO5Lib;

#ifndef ___INo5ObjEvents_DISPINTERFACE_DEFINED__
#define ___INo5ObjEvents_DISPINTERFACE_DEFINED__

/* dispinterface _INo5ObjEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__INo5ObjEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F448E619-7753-4bbd-B1A4-C8646DFA1430")
    _INo5ObjEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _INo5ObjEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _INo5ObjEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _INo5ObjEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _INo5ObjEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _INo5ObjEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _INo5ObjEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _INo5ObjEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _INo5ObjEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _INo5ObjEventsVtbl;

    interface _INo5ObjEvents
    {
        CONST_VTBL struct _INo5ObjEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _INo5ObjEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _INo5ObjEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _INo5ObjEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _INo5ObjEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _INo5ObjEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _INo5ObjEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _INo5ObjEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___INo5ObjEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IChatObj_INTERFACE_DEFINED__
#define __IChatObj_INTERFACE_DEFINED__

/* interface IChatObj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IChatObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C3337C2D-320C-4109-8059-49FA993EA0B5")
    IChatObj : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendMsg( 
            /* [in] */ BSTR msg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE JoinRoom( 
            /* [in] */ BSTR room) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MyName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Room( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Users( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendPrivateMsg( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR msg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Think( 
            /* [in] */ BSTR msg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendEmote( 
            /* [in] */ BSTR msg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Ignore( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT_BOOL bIgnore) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LocalAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsYcht( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RequestCamCookie( 
            /* [in] */ BSTR name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RoomSpace( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VoiceCookie( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChatObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IChatObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IChatObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IChatObj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendMsg )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR msg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *JoinRoom )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR room);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyName )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Room )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Users )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendPrivateMsg )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR msg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Think )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR msg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendEmote )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR msg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Ignore )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT_BOOL bIgnore);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAddress )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocalAddress )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsYcht )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RequestCamCookie )( 
            IChatObj __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RoomSpace )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VoiceCookie )( 
            IChatObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IChatObjVtbl;

    interface IChatObj
    {
        CONST_VTBL struct IChatObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChatObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IChatObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IChatObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IChatObj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IChatObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IChatObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IChatObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IChatObj_SendMsg(This,msg)	\
    (This)->lpVtbl -> SendMsg(This,msg)

#define IChatObj_JoinRoom(This,room)	\
    (This)->lpVtbl -> JoinRoom(This,room)

#define IChatObj_get_MyName(This,pVal)	\
    (This)->lpVtbl -> get_MyName(This,pVal)

#define IChatObj_get_Room(This,pVal)	\
    (This)->lpVtbl -> get_Room(This,pVal)

#define IChatObj_get_Users(This,pVal)	\
    (This)->lpVtbl -> get_Users(This,pVal)

#define IChatObj_SendPrivateMsg(This,name,msg)	\
    (This)->lpVtbl -> SendPrivateMsg(This,name,msg)

#define IChatObj_Think(This,msg)	\
    (This)->lpVtbl -> Think(This,msg)

#define IChatObj_SendEmote(This,msg)	\
    (This)->lpVtbl -> SendEmote(This,msg)

#define IChatObj_Ignore(This,name,bIgnore)	\
    (This)->lpVtbl -> Ignore(This,name,bIgnore)

#define IChatObj_get_RemoteAddress(This,pVal)	\
    (This)->lpVtbl -> get_RemoteAddress(This,pVal)

#define IChatObj_get_LocalAddress(This,pVal)	\
    (This)->lpVtbl -> get_LocalAddress(This,pVal)

#define IChatObj_IsYcht(This,pRes)	\
    (This)->lpVtbl -> IsYcht(This,pRes)

#define IChatObj_RequestCamCookie(This,name)	\
    (This)->lpVtbl -> RequestCamCookie(This,name)

#define IChatObj_get_RoomSpace(This,pVal)	\
    (This)->lpVtbl -> get_RoomSpace(This,pVal)

#define IChatObj_get_VoiceCookie(This,pVal)	\
    (This)->lpVtbl -> get_VoiceCookie(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_SendMsg_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR msg);


void __RPC_STUB IChatObj_SendMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_JoinRoom_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR room);


void __RPC_STUB IChatObj_JoinRoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_MyName_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_MyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_Room_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_Room_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_Users_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_Users_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_SendPrivateMsg_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ BSTR msg);


void __RPC_STUB IChatObj_SendPrivateMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_Think_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR msg);


void __RPC_STUB IChatObj_Think_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_SendEmote_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR msg);


void __RPC_STUB IChatObj_SendEmote_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_Ignore_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ VARIANT_BOOL bIgnore);


void __RPC_STUB IChatObj_Ignore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_RemoteAddress_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_RemoteAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_LocalAddress_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_LocalAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_IsYcht_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pRes);


void __RPC_STUB IChatObj_IsYcht_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatObj_RequestCamCookie_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [in] */ BSTR name);


void __RPC_STUB IChatObj_RequestCamCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_RoomSpace_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_RoomSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatObj_get_VoiceCookie_Proxy( 
    IChatObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IChatObj_get_VoiceCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IChatObj_INTERFACE_DEFINED__ */


#ifndef __IChatsObj_INTERFACE_DEFINED__
#define __IChatsObj_INTERFACE_DEFINED__

/* interface IChatsObj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IChatsObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7AA8C335-A0D3-429C-8579-39E0D1298942")
    IChatsObj : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long n,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChatsObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IChatsObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IChatsObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IChatsObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IChatsObj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IChatsObj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IChatsObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IChatsObj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IChatsObj __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IChatsObj __RPC_FAR * This,
            /* [in] */ long n,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IChatsObjVtbl;

    interface IChatsObj
    {
        CONST_VTBL struct IChatsObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChatsObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IChatsObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IChatsObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IChatsObj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IChatsObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IChatsObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IChatsObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IChatsObj_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IChatsObj_get_Item(This,n,pVal)	\
    (This)->lpVtbl -> get_Item(This,n,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatsObj_get_Count_Proxy( 
    IChatsObj __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IChatsObj_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IChatsObj_get_Item_Proxy( 
    IChatsObj __RPC_FAR * This,
    /* [in] */ long n,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IChatsObj_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IChatsObj_INTERFACE_DEFINED__ */


#ifndef __IUsersObj_INTERFACE_DEFINED__
#define __IUsersObj_INTERFACE_DEFINED__

/* interface IUsersObj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsersObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BC286883-9929-4E89-8193-1EB959149AD4")
    IUsersObj : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long n,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUsersObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IUsersObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IUsersObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IUsersObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IUsersObj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IUsersObj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IUsersObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IUsersObj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IUsersObj __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IUsersObj __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IUsersObj __RPC_FAR * This,
            /* [in] */ long n,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IUsersObjVtbl;

    interface IUsersObj
    {
        CONST_VTBL struct IUsersObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsersObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUsersObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUsersObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUsersObj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IUsersObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IUsersObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IUsersObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IUsersObj_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IUsersObj_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IUsersObj_get_Item(This,n,pVal)	\
    (This)->lpVtbl -> get_Item(This,n,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUsersObj_get_Count_Proxy( 
    IUsersObj __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IUsersObj_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUsersObj_get__NewEnum_Proxy( 
    IUsersObj __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IUsersObj_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUsersObj_get_Item_Proxy( 
    IUsersObj __RPC_FAR * This,
    /* [in] */ long n,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IUsersObj_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUsersObj_INTERFACE_DEFINED__ */


#ifndef __IUserObj_INTERFACE_DEFINED__
#define __IUserObj_INTERFACE_DEFINED__

/* interface IUserObj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUserObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0D30C0D7-CCEB-4C96-8CDB-9EFCEDE1712F")
    IUserObj : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Cam( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Client( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gender( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Age( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Nickname( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IUserObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IUserObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IUserObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IUserObj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IUserObj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IUserObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IUserObj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Cam )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Client )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Gender )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Age )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Nickname )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Location )( 
            IUserObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IUserObjVtbl;

    interface IUserObj
    {
        CONST_VTBL struct IUserObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserObj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IUserObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IUserObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IUserObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IUserObj_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IUserObj_get_Cam(This,pVal)	\
    (This)->lpVtbl -> get_Cam(This,pVal)

#define IUserObj_get_Client(This,pVal)	\
    (This)->lpVtbl -> get_Client(This,pVal)

#define IUserObj_get_Gender(This,pVal)	\
    (This)->lpVtbl -> get_Gender(This,pVal)

#define IUserObj_get_Age(This,pVal)	\
    (This)->lpVtbl -> get_Age(This,pVal)

#define IUserObj_get_Nickname(This,pVal)	\
    (This)->lpVtbl -> get_Nickname(This,pVal)

#define IUserObj_get_Location(This,pVal)	\
    (This)->lpVtbl -> get_Location(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Name_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Cam_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Cam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Client_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Client_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Gender_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Gender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Age_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Age_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Nickname_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Nickname_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IUserObj_get_Location_Proxy( 
    IUserObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IUserObj_get_Location_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserObj_INTERFACE_DEFINED__ */


#ifndef __IYahooTxtObj_INTERFACE_DEFINED__
#define __IYahooTxtObj_INTERFACE_DEFINED__

/* interface IYahooTxtObj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IYahooTxtObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("16354493-1CAD-4890-83CF-2C4A146C7170")
    IYahooTxtObj : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Text( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Plain( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IYahooTxtObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IYahooTxtObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IYahooTxtObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Text )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Plain )( 
            IYahooTxtObj __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IYahooTxtObjVtbl;

    interface IYahooTxtObj
    {
        CONST_VTBL struct IYahooTxtObjVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IYahooTxtObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IYahooTxtObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IYahooTxtObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IYahooTxtObj_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IYahooTxtObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IYahooTxtObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IYahooTxtObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IYahooTxtObj_get_Text(This,pVal)	\
    (This)->lpVtbl -> get_Text(This,pVal)

#define IYahooTxtObj_get_Plain(This,pVal)	\
    (This)->lpVtbl -> get_Plain(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IYahooTxtObj_get_Text_Proxy( 
    IYahooTxtObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IYahooTxtObj_get_Text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IYahooTxtObj_get_Plain_Proxy( 
    IYahooTxtObj __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IYahooTxtObj_get_Plain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IYahooTxtObj_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_No5Obj;

#ifdef __cplusplus

class DECLSPEC_UUID("1730B25E-04C1-45D3-BB75-556C4724BE00")
No5Obj;
#endif

EXTERN_C const CLSID CLSID_ChatObj;

#ifdef __cplusplus

class DECLSPEC_UUID("B953E212-B6FB-4CE5-B1B3-876750DC1BB6")
ChatObj;
#endif

EXTERN_C const CLSID CLSID_ChatsObj;

#ifdef __cplusplus

class DECLSPEC_UUID("63D1A098-13A3-441F-BE0A-32814AB34206")
ChatsObj;
#endif

EXTERN_C const CLSID CLSID_UsersObj;

#ifdef __cplusplus

class DECLSPEC_UUID("ABA47659-FA41-4F9B-90C9-D91511ED122A")
UsersObj;
#endif

EXTERN_C const CLSID CLSID_UserObj;

#ifdef __cplusplus

class DECLSPEC_UUID("58EDF6B9-0570-4891-BF1D-A8453567D1EB")
UserObj;
#endif

EXTERN_C const CLSID CLSID_YahooTxtObj;

#ifdef __cplusplus

class DECLSPEC_UUID("AE28F046-EDB2-4A27-9B00-F1BCA9765260")
YahooTxtObj;
#endif
#endif /* __NO5Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
