 extern ViewAnglesHookCtrl: dq
 extern ViewAnglesHookJmpBack: dq
 extern SetShootspaceJmpBack: dq
 extern SetShootspaceVa: dq

 .code
HkSetViewAngles proc
		movss dword ptr [rsp + 38h], xmm1
		movss dword ptr [r14 + 48h], xmm1
		mov eax, dword ptr [rsp + 38h]
		push rdx
		push rcx
		mov rcx, [ViewAnglesHookCtrl]
		mov edx, -1
		lock xchg [rcx], edx
		cmp edx, -1
		je SET_PITCH
		mov [r14 + 48h], edx
SET_PITCH :
		mov edx, [r14 + 48h]
		mov [rcx + 4], edx
		mov edx, -1
		lock xchg [rcx + 8h], edx
		cmp edx, -1
		je RETURN
		mov [r14 + 44h], edx
RETURN :
		mov edx, [r14 + 44h]
		mov [rcx + 0Ch], edx
		pop rcx
		pop rdx
		jmp qword ptr [ViewAnglesHookJmpBack]
HkSetViewAngles endp

HkSetShootspace proc
	lea rdx, [rbp + 0F0h]
	mov rcx, r13
	mov r8, ViewAnglesHookCtrl
	call SetShootspaceVa
	jmp qword ptr [SetShootspaceJmpBack]
HkSetShootspace endp
end