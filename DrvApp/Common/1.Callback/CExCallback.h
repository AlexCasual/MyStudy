#pragma once

namespace ddk::callback {
	//���Ƹ�ʽ L"\\Callback\\xxxx"
	class CExCallback
	{
	private://˽�б���
		using pfnExCallback = std::function<VOID(PVOID, PVOID)>;
		pfnExCallback m_callback;
		PCALLBACK_OBJECT m_pCallbackObj;
		PVOID m_pRegister;
	public://���졢����������������  
		CExCallback() {
			m_callback = NULL;
			m_pCallbackObj = NULL;
			m_pRegister = NULL;
		}
		CExCallback(WCHAR *callback_name) : CExCallback(){
			Create(callback_name);
		}
		~CExCallback() {
			if (m_pRegister)
				ExUnregisterCallback(m_pRegister);
			if (m_pCallbackObj)
				ObDereferenceObject(m_pCallbackObj);
		}
		CExCallback & operator = (CExCallback &callback) {
			this->m_pCallbackObj = callback.m_pCallbackObj;
			this->m_pRegister    = callback.m_pRegister;
			this->m_callback     = callback.m_callback;
			return (*this);
		}
	public://���������ûص���֪ͨ�ص�
		BOOL Create(WCHAR *callback_name) {
			//û���򴴽��������
			UNICODE_STRING nsCallBackName;
			OBJECT_ATTRIBUTES oa;

			RtlInitUnicodeString(&nsCallBackName, callback_name);
			InitializeObjectAttributes(&oa,
				&nsCallBackName,
				OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE | OBJ_PERMANENT,
				nullptr,
				nullptr);
			auto ns = ExCreateCallback(reinterpret_cast<PCALLBACK_OBJECT*>(&m_pCallbackObj),
				&oa,
				TRUE,
				TRUE);
			if (NT_SUCCESS(ns) && m_pCallbackObj)
				return TRUE;
			ddk::log::StatusInfo::Print(ns);
			return FALSE;
		}
		BOOL SetCallback(pfnExCallback callback) {
			BOOL ret = FALSE;
			do
			{
				if (!m_pCallbackObj || m_pRegister)	//�Ѿ�ע���ֱ�ӷ���FALSE
					break;
				m_pRegister = ExRegisterCallback(m_pCallbackObj, _ExCallback, this);
				if (!m_pRegister)
					break;
				m_callback = callback;
				ret = TRUE;
			} while (true);
			return ret;
		}
		VOID NotifyCallback(PVOID Arg1, PVOID Arg2) {
			ExNotifyCallback(m_pCallbackObj, Arg1, Arg2);
		}
	private:
		static VOID _ExCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2) {
			auto pThis = reinterpret_cast<CExCallback *>(CallbackContext);
			__try {
				pThis->m_callback(Argument1, Argument2);
			}__except(1) {
			}
		}
	};
}