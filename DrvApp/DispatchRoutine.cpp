#include <stdafx.h>
#include "ioctrl.h"

NTSTATUS CreateDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS ReadDispatch  (PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS WriteDispatch (PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS CloseDispatch (PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS HELLO1_HANDLE(PVOID InputBuffer, ULONG InputBufferSize,PVOID OutputBuffer, ULONG OutputBufferSize, ULONG_PTR *ReturnSize);
NTSTATUS HELLO2_HANDLE(PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, ULONG_PTR *ReturnSize);
NTSTATUS HELLO3_HANDLE(PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, ULONG_PTR *ReturnSize);

void setDispatchRoutine() {
	//��ͨ�ַ�����
	g_dev.set_irp_callback(IRP_MJ_CREATE, CreateDispatch);
	g_dev.set_irp_callback(IRP_MJ_READ  , ReadDispatch);
	g_dev.set_irp_callback(IRP_MJ_WRITE , WriteDispatch);
	g_dev.set_irp_callback(IRP_MJ_CLOSE , CloseDispatch);
	//IO_DEVICE_CONTROL�ַ�����
	g_dev.set_ioctrl_callback(FG_HELLO1, HELLO1_HANDLE);	//METHOD_BUFFER��ʽ
	g_dev.set_ioctrl_callback(FG_HELLO2, HELLO2_HANDLE);	//DIRECT_IO��ʽ
	g_dev.set_ioctrl_callback(FG_HELLO3, HELLO3_HANDLE);	//DIRECT_IO��ʽ
}
NTSTATUS CreateDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	LOG_DEBUG("create dispatch\r\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS ReadDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp) {
	LOG_DEBUG("read dispatch\r\n");
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG length = stack->Parameters.Read.Length;                       //��ӦReadFile/WriteFile����
	ULONG mdl_len = MmGetMdlByteCount(pIrp->MdlAddress);                //��������������
	PVOID mdl_address = MmGetMdlVirtualAddress(pIrp->MdlAddress);       //�����������׵�ַ
	ULONG mdl_offset = MmGetMdlByteOffset(pIrp->MdlAddress);            //������������ƫ����
	PVOID kernel_address = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);//�õ�MDL�ں�ģʽ�µ�ӳ��+

	//TODO...
	char *buf = "hello world";
	RtlCopyMemory(kernel_address, buf, strlen(buf) + 1);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = strlen(buf) + 1;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS WriteDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	LOG_DEBUG("write dispatch\r\n");
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG length = stack->Parameters.Write.Length;                       //��ӦReadFile/WriteFile����
	ULONG mdl_len = MmGetMdlByteCount(pIrp->MdlAddress);                //��������������
	PVOID mdl_address = MmGetMdlVirtualAddress(pIrp->MdlAddress);       //�����������׵�ַ
	ULONG mdl_offset = MmGetMdlByteOffset(pIrp->MdlAddress);            //������������ƫ����
	PVOID kernel_address = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);//�õ�MDL�ں�ģʽ�µ�ӳ��+

	//TODO...
	LOG_DEBUG("%s\r\n", kernel_address);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS CloseDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	LOG_DEBUG("Close dispatch\r\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS HELLO1_HANDLE(PVOID InputBuffer,ULONG InputBufferSize,
	PVOID OutputBuffer,ULONG OutputBufferSize,ULONG_PTR *ReturnSize)
{
	char *str = reinterpret_cast<char *>(InputBuffer);
	char *retStr = "����ְ�";
	LOG_DEBUG("%s\r\n", str);
	*ReturnSize = strlen(retStr) + 1;
	RtlCopyMemory(OutputBuffer, retStr, *ReturnSize);
	return STATUS_SUCCESS;
}
NTSTATUS HELLO2_HANDLE(PVOID InputBuffer, ULONG InputBufferSize,
	PVOID OutputBuffer, ULONG OutputBufferSize, ULONG_PTR *ReturnSize) {
	char *str = reinterpret_cast<char *>(InputBuffer);
	char *retStr = "����үү";
	LOG_DEBUG("%s\r\n", str);
	*ReturnSize = strlen(retStr) + 1;
	RtlCopyMemory(OutputBuffer, retStr, *ReturnSize);
	return STATUS_SUCCESS;
}
NTSTATUS HELLO3_HANDLE(PVOID InputBuffer, ULONG InputBufferSize, 
	PVOID OutputBuffer, ULONG OutputBufferSize, ULONG_PTR *ReturnSize) {
	char *str = reinterpret_cast<char *>(InputBuffer);
	char *retStr = "��������";
	LOG_DEBUG("%s\r\n", str);
	*ReturnSize = strlen(retStr) + 1;
	RtlCopyMemory(OutputBuffer, retStr, *ReturnSize);
	return STATUS_SUCCESS;
}