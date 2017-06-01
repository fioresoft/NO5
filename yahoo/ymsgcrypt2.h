/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Dec 09 10:26:56 2004
 */
/* Compiler settings for D:\projects\vc\Atl\ymsgcrypt2\ymsgcrypt2.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
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

#ifndef __ymsgcrypt2_h__
#define __ymsgcrypt2_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IYMsgAuth_FWD_DEFINED__
#define __IYMsgAuth_FWD_DEFINED__
typedef interface IYMsgAuth IYMsgAuth;
#endif 	/* __IYMsgAuth_FWD_DEFINED__ */


#ifndef __YMsgAuth_FWD_DEFINED__
#define __YMsgAuth_FWD_DEFINED__

#ifdef __cplusplus
typedef class YMsgAuth YMsgAuth;
#else
typedef struct YMsgAuth YMsgAuth;
#endif /* __cplusplus */

#endif 	/* __YMsgAuth_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IYMsgAuth_INTERFACE_DEFINED__
#define __IYMsgAuth_INTERFACE_DEFINED__

/* interface IYMsgAuth */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IYMsgAuth;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("859FCE31-4959-11D9-A17B-BD378004423E")
    IYMsgAuth : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Id( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Pw( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Seed( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Result6( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Result96( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Calculate( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IYMsgAuthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IYMsgAuth __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IYMsgAuth __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IYMsgAuth __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Id )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Pw )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Seed )( 
            IYMsgAuth __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Result6 )( 
            IYMsgAuth __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Result96 )( 
            IYMsgAuth __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Calculate )( 
            IYMsgAuth __RPC_FAR * This);
        
        END_INTERFACE
    } IYMsgAuthVtbl;

    interface IYMsgAuth
    {
        CONST_VTBL struct IYMsgAuthVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IYMsgAuth_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IYMsgAuth_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IYMsgAuth_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IYMsgAuth_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IYMsgAuth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IYMsgAuth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IYMsgAuth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IYMsgAuth_put_Id(This,newVal)	\
    (This)->lpVtbl -> put_Id(This,newVal)

#define IYMsgAuth_put_Pw(This,newVal)	\
    (This)->lpVtbl -> put_Pw(This,newVal)

#define IYMsgAuth_put_Seed(This,newVal)	\
    (This)->lpVtbl -> put_Seed(This,newVal)

#define IYMsgAuth_get_Result6(This,pVal)	\
    (This)->lpVtbl -> get_Result6(This,pVal)

#define IYMsgAuth_get_Result96(This,pVal)	\
    (This)->lpVtbl -> get_Result96(This,pVal)

#define IYMsgAuth_Calculate(This)	\
    (This)->lpVtbl -> Calculate(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_put_Id_Proxy( 
    IYMsgAuth __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IYMsgAuth_put_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_put_Pw_Proxy( 
    IYMsgAuth __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IYMsgAuth_put_Pw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_put_Seed_Proxy( 
    IYMsgAuth __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IYMsgAuth_put_Seed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_get_Result6_Proxy( 
    IYMsgAuth __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IYMsgAuth_get_Result6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_get_Result96_Proxy( 
    IYMsgAuth __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IYMsgAuth_get_Result96_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IYMsgAuth_Calculate_Proxy( 
    IYMsgAuth __RPC_FAR * This);


void __RPC_STUB IYMsgAuth_Calculate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IYMsgAuth_INTERFACE_DEFINED__ */



#ifndef __YMSGCRYPT2Lib_LIBRARY_DEFINED__
#define __YMSGCRYPT2Lib_LIBRARY_DEFINED__

/* library YMSGCRYPT2Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_YMSGCRYPT2Lib;

EXTERN_C const CLSID CLSID_YMsgAuth;

#ifdef __cplusplus

class DECLSPEC_UUID("859FCE32-4959-11D9-A17B-BD378004423E")
YMsgAuth;
#endif
#endif /* __YMSGCRYPT2Lib_LIBRARY_DEFINED__ */

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
