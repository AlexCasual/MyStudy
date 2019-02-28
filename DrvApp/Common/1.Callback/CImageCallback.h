#pragma once

namespace ddk::callback {
	class CImageCallback
	{
	private:
		using pfnImageCallback = std::function<VOID(PUNICODE_STRING, HANDLE, PIMAGE_INFO)>;
		pfnImageCallback m_callback;

	private://���ص����ķ���
		CImageCallback() {
			PsSetLoadImageNotifyRoutine(ImageCallback);
		}
		CImageCallback(CImageCallback &) = delete;
		CImageCallback& operator = (CImageCallback) = delete;
	public://���������ķ���
		~CImageCallback() {
			PsRemoveLoadImageNotifyRoutine(ImageCallback);
		}
		static CImageCallback &getIntanceRef() {
			static CImageCallback self;
			return self;
		}
		static CImageCallback *getInstancePtr() {
			return &getIntanceRef();
		}
		void RegCallback(pfnImageCallback callback) {
			m_callback = callback;
		}
	private:
		static VOID ImageCallback(PUNICODE_STRING  FullImageName, HANDLE  ProcessId, PIMAGE_INFO  ImageInfo) {
			getInstancePtr()->m_callback(FullImageName, ProcessId, ImageInfo);
		}
	};
}