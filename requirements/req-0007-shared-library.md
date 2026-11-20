# REQ-0007 — Shared library as a COM server

**Content type:** Project requirement

**Status:** Proposed

## Statement

In the shared build (`WFC_MSCOMCTL2_SHARED=ON`) the library is
`wfcmscomctl2.dll`.

- **Entry points.** The DLL shall export `DllCanUnloadNow` (S_OK while no
  object is alive), `DllGetClassObject`, `DllRegisterServer` and
  `DllUnregisterServer`. `DllGetClassObject` for an unknown class answers
  `CLASS_E_CLASSNOTAVAILABLE`.
- **Version resource.** The file version shall equal `VERSION` and the
  `Comments` string shall be `Targets <TARGET>` (the Microsoft component the
  library stands in for).
- **Registration.** `DllRegisterServer` shall write
  `Software\WFC\Libraries\mscomctl2` (the version, the target and the path of
  the server) and, for every class, the WFC ProgID (`WFC.MSComCtl2.<Class>`)
  with its class id and an `InprocServer32` key that names the DLL.
  `WFC_REGISTER_SCOPE=user` selects the registry of the current user.
  With `WFC_REGISTER_MICROSOFT_IDS=1` the Microsoft ProgIDs and class ids of
  the type library are registered as well. `DllUnregisterServer` shall remove
  everything the registration wrote.
- **Objects.** `DllGetClassObject` for a class shall return a class factory
  whose `CreateInstance` makes a late-bound (`IDispatch`) control of the
  member table: a property keeps the value it is given, a method is accepted,
  an unknown name is refused, and aggregation is refused
  (`CLASS_E_NOAGGREGATION`). The UpDown, FlatScrollBar, MonthView and
  DTPicker objects follow the rules of REQ-0001 to REQ-0004 for their
  defaults, ranges (error 380 outside them), parts of `Value`, and `Change`
  through a connection point.

## Known limits

The test leaves the registry alone and passes with the exit code 77 (skipped)
when the registry of the current user already holds the library; the part
that uses the Microsoft ids is skipped when a Microsoft class id is already
in `HKCU`.

## Verification

Method: automated test.

- [TC-0006](../tests/tc-0006-dll.tex): the C program that loads the DLL.

## Relationships

REQ-0001 to REQ-0004 give the behaviour the COM objects follow; REQ-0006 is
the member table behind them.
