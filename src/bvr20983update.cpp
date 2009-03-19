/*
 * $Id: $
 * 
 * Copyright (C) 2008-2009 Dorothea Wachmann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "os.h"
#include "util/registry.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/versioninfo.h"
#include "util/apputil.h"
#include "util/backgroundtransfer.h"
#include "util/md5sum.h"
#include <sstream>
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"

using namespace bvr20983;
using namespace bvr20983::util;

/**
 *
 */
BOOL WINAPI DllMain(HINSTANCE hDllInst,DWORD fdwReason,LPVOID lpvReserved)
{ BOOL bResult = TRUE;

  switch( fdwReason )
  { case DLL_PROCESS_ATTACH:
      { VersionInfo verInfo((HMODULE)hDllInst);

        LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));

      }
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
  }

  return bResult;
} // of DllMain


#ifdef _UNICODE
#define _BitsAdmin_ BitsAdminW
#else
#define _BitsAdmin_ BitsAdminA
#endif

/**
 *
 */
void PrintBtxCreateJob(HWND hwnd)
{ basic_ostringstream<TCHAR> msgStream;
  msgStream<<_T("Usage: rundll32 <dllname>,BitsAdmin <command> args")<<endl;
  msgStream<<_T("       create <jobname>: create a bits job")<<endl;

  TString msg = msgStream.str();

  ::MessageBox(hwnd,msg.c_str(),_T("BitsAdmin"),MB_OK | MB_ICONINFORMATION);
} // of PrintBtxCreateJob()

/**
 *
 */
void ParseCommandLine(LPWSTR lpszCmdLine,VTString& args)
{ int     i         = 0;
  LPTSTR  nextToken = NULL;
 
  for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    args.push_back(tok);
} // of ParseCommandline()

/**
 *
 */
static LPTSTR bgStates[] =
{ _T("QUEUED"),
	_T("CONNECTING"),
	_T("TRANSFERRING"),
	_T("SUSPENDED"),
	_T("ERROR"),
	_T("TRANSIENT_ERROR"),
	_T("TRANSFERRED"),
	_T("ACKNOWLEDGED"),
	_T("CANCELLED")
};

/**
 *
 */
STDAPI_(void) _BitsAdmin_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ ::CoInitialize(NULL);

  try
  { auto_ptr<BackgroundTransfer> btx(new BackgroundTransfer());

    VTString args;
    ParseCommandLine(lpszCmdLine,args);

    if( args.empty() )
      PrintBtxCreateJob(hwnd);
    else
    {
      TString command = args[0];

      args.erase( args.begin( ) );

      if( _tcscmp(command.c_str(),_T("create"))==0 )
      { CGUID jobId;

        btx->CreateJob(args[0].c_str(),jobId);
      } // of if
      else if( _tcscmp(command.c_str(),_T("list"))==0 )
        btx->List();
      else if( _tcscmp(command.c_str(),_T("update"))==0 && args.size()>=3 )
      { CGUID                      jobId;
        COMPtr<IBackgroundCopyJob> job;

        if( !btx->GetJob(args[0].c_str(),jobId) )
        { btx->CreateJob(args[0].c_str(),jobId);

          COMPtr<IBackgroundCopyJob> newJob;

          if( btx->GetJob(jobId,newJob) )
            THROW_COMEXCEPTION( newJob->AddFile(args[1].c_str(),args[2].c_str()) );
        } // of if

        if( btx->GetJob(jobId,job) )
        { BG_JOB_STATE    jobState;
          BG_JOB_PROGRESS jobProgress;

          THROW_COMEXCEPTION( job->GetState(&jobState) ); 

          THROW_COMEXCEPTION( job->GetProgress(&jobProgress) ); 

          OutputDebugFmt(_T("BitsAdmin(): jobstate=%s %I64u / %I64u "),
                         bgStates[jobState],
                         jobProgress.BytesTransferred,jobProgress.BytesTotal
                        );

          switch( jobState )
          {
          case BG_JOB_STATE_SUSPENDED:
            THROW_COMEXCEPTION( job->Resume() ); 
            break;
          case BG_JOB_STATE_TRANSFERRED:
            { THROW_COMEXCEPTION( job->Complete() );
              
              COMPtr<IEnumBackgroundCopyFiles> files;

              THROW_COMEXCEPTION( job->EnumFiles(&files) );

              for( ;; )
              { COMPtr<IBackgroundCopyFile> aFile;

                if( S_OK==files->Next(1,&aFile,NULL) )
                { LPTSTR localFileName = NULL;

                  if( SUCCEEDED(aFile->GetLocalName(&localFileName)) )
                  { MD5Sum               md5sum;
                    auto_ptr<CryptoHash> hash;
                    auto_ptr<BYTE>       hashValue;
                    DWORD                hashValueLen;

                    md5sum.CalcFileHash(localFileName,hash);

                    CryptoHash* pHash = hash.get();

                    hashValueLen = pHash->Get(hashValue);

                    if( hashValueLen>0 )
                    { basic_ostringstream<TCHAR> hashValueStr;

                      hashValueStr<<setfill(_T('0'))<<setw(2)<<hex;

                      BYTE* pBuffer = hashValue.get();

                      for( DWORD i=0;i<hashValueLen;i++ )
                        hashValueStr<<pBuffer[i];

                      OutputDebugFmt(_T("%s: md5{%ld}[%s]\n"),localFileName,hashValueLen,hashValueStr.str());
                    } // of if

                    ::CoTaskMemFree(localFileName);
                  } // of if
                } // of if
                else
                  break;
              } // of for
            }
            break;
          case BG_JOB_STATE_ERROR:
            THROW_COMEXCEPTION( job->Cancel() ); 
            break;
          default:
            break;
          } // of switch()
        } // of if
      } // of else if
      else
        PrintBtxCreateJob(hwnd);
    } // of if
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("BitsAdmin(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("BitsAdmin(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("BitsAdmin(): Exception\n")); }

  ::CoUninitialize();
} // of _BitsCreateJob_()
/*==========================END-OF-FILE===================================*/
