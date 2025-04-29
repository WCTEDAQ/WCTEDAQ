// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME WCTE_RootDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "/root/WCTEDAQ/DataModel/DAQInfo.h"
#include "/root/WCTEDAQ/DataModel/ReadoutWindow.h"
#include "/root/WCTEDAQ/DataModel/WCTEMPMTPPS.h"
#include "/root/WCTEDAQ/DataModel/TDCHit.h"
#include "/root/WCTEDAQ/DataModel/QDCHit.h"
#include "/root/WCTEDAQ/DataModel/MPMTMessages.h"
#include "/root/WCTEDAQ/DataModel/MPMTMessageData.h"
#include "/root/WCTEDAQ/DataModel/MPMTWaveformSamples.h"
#include "/root/WCTEDAQ/DataModel/TriggerType.h"
#include "SerialisableObject.h"
#include "SerialisableObject.h"
#include "TriggerType.h"
#include "DAQInfo.h"
#include "ReadoutWindow.h"
#include "MPMTMessages.h"
#include "MPMTWaveformSamples.h"
#include "MPMTMessageData.h"
#include "WCTEMPMTPPS.h"
#include "TDCHit.h"
#include "QDCHit.h"
#include "BitFunctions.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *SerialisableObject_Dictionary();
   static void SerialisableObject_TClassManip(TClass*);
   static void delete_SerialisableObject(void *p);
   static void deleteArray_SerialisableObject(void *p);
   static void destruct_SerialisableObject(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SerialisableObject*)
   {
      ::SerialisableObject *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SerialisableObject));
      static ::ROOT::TGenericClassInfo 
         instance("SerialisableObject", "SerialisableObject.h", 3,
                  typeid(::SerialisableObject), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SerialisableObject_Dictionary, isa_proxy, 1,
                  sizeof(::SerialisableObject) );
      instance.SetDelete(&delete_SerialisableObject);
      instance.SetDeleteArray(&deleteArray_SerialisableObject);
      instance.SetDestructor(&destruct_SerialisableObject);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SerialisableObject*)
   {
      return GenerateInitInstanceLocal((::SerialisableObject*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SerialisableObject*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SerialisableObject_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SerialisableObject*)0x0)->GetClass();
      SerialisableObject_TClassManip(theClass);
   return theClass;
   }

   static void SerialisableObject_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *WCTEMPMTPPS_Dictionary();
   static void WCTEMPMTPPS_TClassManip(TClass*);
   static void *new_WCTEMPMTPPS(void *p = 0);
   static void *newArray_WCTEMPMTPPS(Long_t size, void *p);
   static void delete_WCTEMPMTPPS(void *p);
   static void deleteArray_WCTEMPMTPPS(void *p);
   static void destruct_WCTEMPMTPPS(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::WCTEMPMTPPS*)
   {
      ::WCTEMPMTPPS *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::WCTEMPMTPPS));
      static ::ROOT::TGenericClassInfo 
         instance("WCTEMPMTPPS", "WCTEMPMTPPS.h", 13,
                  typeid(::WCTEMPMTPPS), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &WCTEMPMTPPS_Dictionary, isa_proxy, 4,
                  sizeof(::WCTEMPMTPPS) );
      instance.SetNew(&new_WCTEMPMTPPS);
      instance.SetNewArray(&newArray_WCTEMPMTPPS);
      instance.SetDelete(&delete_WCTEMPMTPPS);
      instance.SetDeleteArray(&deleteArray_WCTEMPMTPPS);
      instance.SetDestructor(&destruct_WCTEMPMTPPS);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::WCTEMPMTPPS*)
   {
      return GenerateInitInstanceLocal((::WCTEMPMTPPS*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::WCTEMPMTPPS*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *WCTEMPMTPPS_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::WCTEMPMTPPS*)0x0)->GetClass();
      WCTEMPMTPPS_TClassManip(theClass);
   return theClass;
   }

   static void WCTEMPMTPPS_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MPMTLED_Dictionary();
   static void MPMTLED_TClassManip(TClass*);
   static void *new_MPMTLED(void *p = 0);
   static void *newArray_MPMTLED(Long_t size, void *p);
   static void delete_MPMTLED(void *p);
   static void deleteArray_MPMTLED(void *p);
   static void destruct_MPMTLED(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MPMTLED*)
   {
      ::MPMTLED *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MPMTLED));
      static ::ROOT::TGenericClassInfo 
         instance("MPMTLED", "TriggerType.h", 18,
                  typeid(::MPMTLED), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MPMTLED_Dictionary, isa_proxy, 4,
                  sizeof(::MPMTLED) );
      instance.SetNew(&new_MPMTLED);
      instance.SetNewArray(&newArray_MPMTLED);
      instance.SetDelete(&delete_MPMTLED);
      instance.SetDeleteArray(&deleteArray_MPMTLED);
      instance.SetDestructor(&destruct_MPMTLED);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MPMTLED*)
   {
      return GenerateInitInstanceLocal((::MPMTLED*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MPMTLED*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MPMTLED_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MPMTLED*)0x0)->GetClass();
      MPMTLED_TClassManip(theClass);
   return theClass;
   }

   static void MPMTLED_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *P_MPMTLED_Dictionary();
   static void P_MPMTLED_TClassManip(TClass*);
   static void *new_P_MPMTLED(void *p = 0);
   static void *newArray_P_MPMTLED(Long_t size, void *p);
   static void delete_P_MPMTLED(void *p);
   static void deleteArray_P_MPMTLED(void *p);
   static void destruct_P_MPMTLED(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::P_MPMTLED*)
   {
      ::P_MPMTLED *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::P_MPMTLED));
      static ::ROOT::TGenericClassInfo 
         instance("P_MPMTLED", "TriggerType.h", 78,
                  typeid(::P_MPMTLED), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &P_MPMTLED_Dictionary, isa_proxy, 4,
                  sizeof(::P_MPMTLED) );
      instance.SetNew(&new_P_MPMTLED);
      instance.SetNewArray(&newArray_P_MPMTLED);
      instance.SetDelete(&delete_P_MPMTLED);
      instance.SetDeleteArray(&deleteArray_P_MPMTLED);
      instance.SetDestructor(&destruct_P_MPMTLED);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::P_MPMTLED*)
   {
      return GenerateInitInstanceLocal((::P_MPMTLED*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::P_MPMTLED*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *P_MPMTLED_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::P_MPMTLED*)0x0)->GetClass();
      P_MPMTLED_TClassManip(theClass);
   return theClass;
   }

   static void P_MPMTLED_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TriggerInfo_Dictionary();
   static void TriggerInfo_TClassManip(TClass*);
   static void *new_TriggerInfo(void *p = 0);
   static void *newArray_TriggerInfo(Long_t size, void *p);
   static void delete_TriggerInfo(void *p);
   static void deleteArray_TriggerInfo(void *p);
   static void destruct_TriggerInfo(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TriggerInfo*)
   {
      ::TriggerInfo *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TriggerInfo));
      static ::ROOT::TGenericClassInfo 
         instance("TriggerInfo", "TriggerType.h", 115,
                  typeid(::TriggerInfo), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TriggerInfo_Dictionary, isa_proxy, 4,
                  sizeof(::TriggerInfo) );
      instance.SetNew(&new_TriggerInfo);
      instance.SetNewArray(&newArray_TriggerInfo);
      instance.SetDelete(&delete_TriggerInfo);
      instance.SetDeleteArray(&deleteArray_TriggerInfo);
      instance.SetDestructor(&destruct_TriggerInfo);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TriggerInfo*)
   {
      return GenerateInitInstanceLocal((::TriggerInfo*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TriggerInfo*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TriggerInfo_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TriggerInfo*)0x0)->GetClass();
      TriggerInfo_TClassManip(theClass);
   return theClass;
   }

   static void TriggerInfo_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *DAQInfo_Dictionary();
   static void DAQInfo_TClassManip(TClass*);
   static void *new_DAQInfo(void *p = 0);
   static void *newArray_DAQInfo(Long_t size, void *p);
   static void delete_DAQInfo(void *p);
   static void deleteArray_DAQInfo(void *p);
   static void destruct_DAQInfo(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DAQInfo*)
   {
      ::DAQInfo *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::DAQInfo));
      static ::ROOT::TGenericClassInfo 
         instance("DAQInfo", "DAQInfo.h", 11,
                  typeid(::DAQInfo), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &DAQInfo_Dictionary, isa_proxy, 4,
                  sizeof(::DAQInfo) );
      instance.SetNew(&new_DAQInfo);
      instance.SetNewArray(&newArray_DAQInfo);
      instance.SetDelete(&delete_DAQInfo);
      instance.SetDeleteArray(&deleteArray_DAQInfo);
      instance.SetDestructor(&destruct_DAQInfo);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DAQInfo*)
   {
      return GenerateInitInstanceLocal((::DAQInfo*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::DAQInfo*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *DAQInfo_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::DAQInfo*)0x0)->GetClass();
      DAQInfo_TClassManip(theClass);
   return theClass;
   }

   static void DAQInfo_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TDCHit_Dictionary();
   static void TDCHit_TClassManip(TClass*);
   static void *new_TDCHit(void *p = 0);
   static void *newArray_TDCHit(Long_t size, void *p);
   static void delete_TDCHit(void *p);
   static void deleteArray_TDCHit(void *p);
   static void destruct_TDCHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TDCHit*)
   {
      ::TDCHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TDCHit));
      static ::ROOT::TGenericClassInfo 
         instance("TDCHit", "TDCHit.h", 45,
                  typeid(::TDCHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TDCHit_Dictionary, isa_proxy, 4,
                  sizeof(::TDCHit) );
      instance.SetNew(&new_TDCHit);
      instance.SetNewArray(&newArray_TDCHit);
      instance.SetDelete(&delete_TDCHit);
      instance.SetDeleteArray(&deleteArray_TDCHit);
      instance.SetDestructor(&destruct_TDCHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TDCHit*)
   {
      return GenerateInitInstanceLocal((::TDCHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TDCHit*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TDCHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TDCHit*)0x0)->GetClass();
      TDCHit_TClassManip(theClass);
   return theClass;
   }

   static void TDCHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *QDCHit_Dictionary();
   static void QDCHit_TClassManip(TClass*);
   static void *new_QDCHit(void *p = 0);
   static void *newArray_QDCHit(Long_t size, void *p);
   static void delete_QDCHit(void *p);
   static void deleteArray_QDCHit(void *p);
   static void destruct_QDCHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::QDCHit*)
   {
      ::QDCHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::QDCHit));
      static ::ROOT::TGenericClassInfo 
         instance("QDCHit", "QDCHit.h", 38,
                  typeid(::QDCHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &QDCHit_Dictionary, isa_proxy, 4,
                  sizeof(::QDCHit) );
      instance.SetNew(&new_QDCHit);
      instance.SetNewArray(&newArray_QDCHit);
      instance.SetDelete(&delete_QDCHit);
      instance.SetDeleteArray(&deleteArray_QDCHit);
      instance.SetDestructor(&destruct_QDCHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::QDCHit*)
   {
      return GenerateInitInstanceLocal((::QDCHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::QDCHit*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *QDCHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::QDCHit*)0x0)->GetClass();
      QDCHit_TClassManip(theClass);
   return theClass;
   }

   static void QDCHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MPMTWaveformHeader_Dictionary();
   static void MPMTWaveformHeader_TClassManip(TClass*);
   static void *new_MPMTWaveformHeader(void *p = 0);
   static void *newArray_MPMTWaveformHeader(Long_t size, void *p);
   static void delete_MPMTWaveformHeader(void *p);
   static void deleteArray_MPMTWaveformHeader(void *p);
   static void destruct_MPMTWaveformHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MPMTWaveformHeader*)
   {
      ::MPMTWaveformHeader *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MPMTWaveformHeader));
      static ::ROOT::TGenericClassInfo 
         instance("MPMTWaveformHeader", "MPMTMessages.h", 18,
                  typeid(::MPMTWaveformHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MPMTWaveformHeader_Dictionary, isa_proxy, 4,
                  sizeof(::MPMTWaveformHeader) );
      instance.SetNew(&new_MPMTWaveformHeader);
      instance.SetNewArray(&newArray_MPMTWaveformHeader);
      instance.SetDelete(&delete_MPMTWaveformHeader);
      instance.SetDeleteArray(&deleteArray_MPMTWaveformHeader);
      instance.SetDestructor(&destruct_MPMTWaveformHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MPMTWaveformHeader*)
   {
      return GenerateInitInstanceLocal((::MPMTWaveformHeader*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MPMTWaveformHeader*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MPMTWaveformHeader_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MPMTWaveformHeader*)0x0)->GetClass();
      MPMTWaveformHeader_TClassManip(theClass);
   return theClass;
   }

   static void MPMTWaveformHeader_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *P_MPMTWaveformHeader_Dictionary();
   static void P_MPMTWaveformHeader_TClassManip(TClass*);
   static void *new_P_MPMTWaveformHeader(void *p = 0);
   static void *newArray_P_MPMTWaveformHeader(Long_t size, void *p);
   static void delete_P_MPMTWaveformHeader(void *p);
   static void deleteArray_P_MPMTWaveformHeader(void *p);
   static void destruct_P_MPMTWaveformHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::P_MPMTWaveformHeader*)
   {
      ::P_MPMTWaveformHeader *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::P_MPMTWaveformHeader));
      static ::ROOT::TGenericClassInfo 
         instance("P_MPMTWaveformHeader", "MPMTMessages.h", 78,
                  typeid(::P_MPMTWaveformHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &P_MPMTWaveformHeader_Dictionary, isa_proxy, 4,
                  sizeof(::P_MPMTWaveformHeader) );
      instance.SetNew(&new_P_MPMTWaveformHeader);
      instance.SetNewArray(&newArray_P_MPMTWaveformHeader);
      instance.SetDelete(&delete_P_MPMTWaveformHeader);
      instance.SetDeleteArray(&deleteArray_P_MPMTWaveformHeader);
      instance.SetDestructor(&destruct_P_MPMTWaveformHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::P_MPMTWaveformHeader*)
   {
      return GenerateInitInstanceLocal((::P_MPMTWaveformHeader*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::P_MPMTWaveformHeader*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *P_MPMTWaveformHeader_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::P_MPMTWaveformHeader*)0x0)->GetClass();
      P_MPMTWaveformHeader_TClassManip(theClass);
   return theClass;
   }

   static void P_MPMTWaveformHeader_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MPMTHit_Dictionary();
   static void MPMTHit_TClassManip(TClass*);
   static void *new_MPMTHit(void *p = 0);
   static void *newArray_MPMTHit(Long_t size, void *p);
   static void delete_MPMTHit(void *p);
   static void deleteArray_MPMTHit(void *p);
   static void destruct_MPMTHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MPMTHit*)
   {
      ::MPMTHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MPMTHit));
      static ::ROOT::TGenericClassInfo 
         instance("MPMTHit", "MPMTMessages.h", 125,
                  typeid(::MPMTHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MPMTHit_Dictionary, isa_proxy, 4,
                  sizeof(::MPMTHit) );
      instance.SetNew(&new_MPMTHit);
      instance.SetNewArray(&newArray_MPMTHit);
      instance.SetDelete(&delete_MPMTHit);
      instance.SetDeleteArray(&deleteArray_MPMTHit);
      instance.SetDestructor(&destruct_MPMTHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MPMTHit*)
   {
      return GenerateInitInstanceLocal((::MPMTHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MPMTHit*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MPMTHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MPMTHit*)0x0)->GetClass();
      MPMTHit_TClassManip(theClass);
   return theClass;
   }

   static void MPMTHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *P_MPMTHit_Dictionary();
   static void P_MPMTHit_TClassManip(TClass*);
   static void *new_P_MPMTHit(void *p = 0);
   static void *newArray_P_MPMTHit(Long_t size, void *p);
   static void delete_P_MPMTHit(void *p);
   static void deleteArray_P_MPMTHit(void *p);
   static void destruct_P_MPMTHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::P_MPMTHit*)
   {
      ::P_MPMTHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::P_MPMTHit));
      static ::ROOT::TGenericClassInfo 
         instance("P_MPMTHit", "MPMTMessages.h", 176,
                  typeid(::P_MPMTHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &P_MPMTHit_Dictionary, isa_proxy, 4,
                  sizeof(::P_MPMTHit) );
      instance.SetNew(&new_P_MPMTHit);
      instance.SetNewArray(&newArray_P_MPMTHit);
      instance.SetDelete(&delete_P_MPMTHit);
      instance.SetDeleteArray(&deleteArray_P_MPMTHit);
      instance.SetDestructor(&destruct_P_MPMTHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::P_MPMTHit*)
   {
      return GenerateInitInstanceLocal((::P_MPMTHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::P_MPMTHit*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *P_MPMTHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::P_MPMTHit*)0x0)->GetClass();
      P_MPMTHit_TClassManip(theClass);
   return theClass;
   }

   static void P_MPMTHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *PReadoutWindow_Dictionary();
   static void PReadoutWindow_TClassManip(TClass*);
   static void *new_PReadoutWindow(void *p = 0);
   static void *newArray_PReadoutWindow(Long_t size, void *p);
   static void delete_PReadoutWindow(void *p);
   static void deleteArray_PReadoutWindow(void *p);
   static void destruct_PReadoutWindow(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PReadoutWindow*)
   {
      ::PReadoutWindow *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PReadoutWindow));
      static ::ROOT::TGenericClassInfo 
         instance("PReadoutWindow", "ReadoutWindow.h", 82,
                  typeid(::PReadoutWindow), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PReadoutWindow_Dictionary, isa_proxy, 4,
                  sizeof(::PReadoutWindow) );
      instance.SetNew(&new_PReadoutWindow);
      instance.SetNewArray(&newArray_PReadoutWindow);
      instance.SetDelete(&delete_PReadoutWindow);
      instance.SetDeleteArray(&deleteArray_PReadoutWindow);
      instance.SetDestructor(&destruct_PReadoutWindow);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PReadoutWindow*)
   {
      return GenerateInitInstanceLocal((::PReadoutWindow*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PReadoutWindow*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PReadoutWindow_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PReadoutWindow*)0x0)->GetClass();
      PReadoutWindow_TClassManip(theClass);
   return theClass;
   }

   static void PReadoutWindow_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MPMTWaveformSamples_Dictionary();
   static void MPMTWaveformSamples_TClassManip(TClass*);
   static void *new_MPMTWaveformSamples(void *p = 0);
   static void *newArray_MPMTWaveformSamples(Long_t size, void *p);
   static void delete_MPMTWaveformSamples(void *p);
   static void deleteArray_MPMTWaveformSamples(void *p);
   static void destruct_MPMTWaveformSamples(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MPMTWaveformSamples*)
   {
      ::MPMTWaveformSamples *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MPMTWaveformSamples));
      static ::ROOT::TGenericClassInfo 
         instance("MPMTWaveformSamples", "MPMTWaveformSamples.h", 5,
                  typeid(::MPMTWaveformSamples), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MPMTWaveformSamples_Dictionary, isa_proxy, 4,
                  sizeof(::MPMTWaveformSamples) );
      instance.SetNew(&new_MPMTWaveformSamples);
      instance.SetNewArray(&newArray_MPMTWaveformSamples);
      instance.SetDelete(&delete_MPMTWaveformSamples);
      instance.SetDeleteArray(&deleteArray_MPMTWaveformSamples);
      instance.SetDestructor(&destruct_MPMTWaveformSamples);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MPMTWaveformSamples*)
   {
      return GenerateInitInstanceLocal((::MPMTWaveformSamples*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MPMTWaveformSamples*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MPMTWaveformSamples_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MPMTWaveformSamples*)0x0)->GetClass();
      MPMTWaveformSamples_TClassManip(theClass);
   return theClass;
   }

   static void MPMTWaveformSamples_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *MPMTMessageData_Dictionary();
   static void MPMTMessageData_TClassManip(TClass*);
   static void *new_MPMTMessageData(void *p = 0);
   static void *newArray_MPMTMessageData(Long_t size, void *p);
   static void delete_MPMTMessageData(void *p);
   static void deleteArray_MPMTMessageData(void *p);
   static void destruct_MPMTMessageData(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MPMTMessageData*)
   {
      ::MPMTMessageData *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MPMTMessageData));
      static ::ROOT::TGenericClassInfo 
         instance("MPMTMessageData", "MPMTMessageData.h", 5,
                  typeid(::MPMTMessageData), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MPMTMessageData_Dictionary, isa_proxy, 4,
                  sizeof(::MPMTMessageData) );
      instance.SetNew(&new_MPMTMessageData);
      instance.SetNewArray(&newArray_MPMTMessageData);
      instance.SetDelete(&delete_MPMTMessageData);
      instance.SetDeleteArray(&deleteArray_MPMTMessageData);
      instance.SetDestructor(&destruct_MPMTMessageData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MPMTMessageData*)
   {
      return GenerateInitInstanceLocal((::MPMTMessageData*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MPMTMessageData*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MPMTMessageData_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MPMTMessageData*)0x0)->GetClass();
      MPMTMessageData_TClassManip(theClass);
   return theClass;
   }

   static void MPMTMessageData_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_SerialisableObject(void *p) {
      delete ((::SerialisableObject*)p);
   }
   static void deleteArray_SerialisableObject(void *p) {
      delete [] ((::SerialisableObject*)p);
   }
   static void destruct_SerialisableObject(void *p) {
      typedef ::SerialisableObject current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SerialisableObject

namespace ROOT {
   // Wrappers around operator new
   static void *new_WCTEMPMTPPS(void *p) {
      return  p ? new(p) ::WCTEMPMTPPS : new ::WCTEMPMTPPS;
   }
   static void *newArray_WCTEMPMTPPS(Long_t nElements, void *p) {
      return p ? new(p) ::WCTEMPMTPPS[nElements] : new ::WCTEMPMTPPS[nElements];
   }
   // Wrapper around operator delete
   static void delete_WCTEMPMTPPS(void *p) {
      delete ((::WCTEMPMTPPS*)p);
   }
   static void deleteArray_WCTEMPMTPPS(void *p) {
      delete [] ((::WCTEMPMTPPS*)p);
   }
   static void destruct_WCTEMPMTPPS(void *p) {
      typedef ::WCTEMPMTPPS current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::WCTEMPMTPPS

namespace ROOT {
   // Wrappers around operator new
   static void *new_MPMTLED(void *p) {
      return  p ? new(p) ::MPMTLED : new ::MPMTLED;
   }
   static void *newArray_MPMTLED(Long_t nElements, void *p) {
      return p ? new(p) ::MPMTLED[nElements] : new ::MPMTLED[nElements];
   }
   // Wrapper around operator delete
   static void delete_MPMTLED(void *p) {
      delete ((::MPMTLED*)p);
   }
   static void deleteArray_MPMTLED(void *p) {
      delete [] ((::MPMTLED*)p);
   }
   static void destruct_MPMTLED(void *p) {
      typedef ::MPMTLED current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MPMTLED

namespace ROOT {
   // Wrappers around operator new
   static void *new_P_MPMTLED(void *p) {
      return  p ? new(p) ::P_MPMTLED : new ::P_MPMTLED;
   }
   static void *newArray_P_MPMTLED(Long_t nElements, void *p) {
      return p ? new(p) ::P_MPMTLED[nElements] : new ::P_MPMTLED[nElements];
   }
   // Wrapper around operator delete
   static void delete_P_MPMTLED(void *p) {
      delete ((::P_MPMTLED*)p);
   }
   static void deleteArray_P_MPMTLED(void *p) {
      delete [] ((::P_MPMTLED*)p);
   }
   static void destruct_P_MPMTLED(void *p) {
      typedef ::P_MPMTLED current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::P_MPMTLED

namespace ROOT {
   // Wrappers around operator new
   static void *new_TriggerInfo(void *p) {
      return  p ? new(p) ::TriggerInfo : new ::TriggerInfo;
   }
   static void *newArray_TriggerInfo(Long_t nElements, void *p) {
      return p ? new(p) ::TriggerInfo[nElements] : new ::TriggerInfo[nElements];
   }
   // Wrapper around operator delete
   static void delete_TriggerInfo(void *p) {
      delete ((::TriggerInfo*)p);
   }
   static void deleteArray_TriggerInfo(void *p) {
      delete [] ((::TriggerInfo*)p);
   }
   static void destruct_TriggerInfo(void *p) {
      typedef ::TriggerInfo current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TriggerInfo

namespace ROOT {
   // Wrappers around operator new
   static void *new_DAQInfo(void *p) {
      return  p ? new(p) ::DAQInfo : new ::DAQInfo;
   }
   static void *newArray_DAQInfo(Long_t nElements, void *p) {
      return p ? new(p) ::DAQInfo[nElements] : new ::DAQInfo[nElements];
   }
   // Wrapper around operator delete
   static void delete_DAQInfo(void *p) {
      delete ((::DAQInfo*)p);
   }
   static void deleteArray_DAQInfo(void *p) {
      delete [] ((::DAQInfo*)p);
   }
   static void destruct_DAQInfo(void *p) {
      typedef ::DAQInfo current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::DAQInfo

namespace ROOT {
   // Wrappers around operator new
   static void *new_TDCHit(void *p) {
      return  p ? new(p) ::TDCHit : new ::TDCHit;
   }
   static void *newArray_TDCHit(Long_t nElements, void *p) {
      return p ? new(p) ::TDCHit[nElements] : new ::TDCHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_TDCHit(void *p) {
      delete ((::TDCHit*)p);
   }
   static void deleteArray_TDCHit(void *p) {
      delete [] ((::TDCHit*)p);
   }
   static void destruct_TDCHit(void *p) {
      typedef ::TDCHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TDCHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_QDCHit(void *p) {
      return  p ? new(p) ::QDCHit : new ::QDCHit;
   }
   static void *newArray_QDCHit(Long_t nElements, void *p) {
      return p ? new(p) ::QDCHit[nElements] : new ::QDCHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_QDCHit(void *p) {
      delete ((::QDCHit*)p);
   }
   static void deleteArray_QDCHit(void *p) {
      delete [] ((::QDCHit*)p);
   }
   static void destruct_QDCHit(void *p) {
      typedef ::QDCHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::QDCHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_MPMTWaveformHeader(void *p) {
      return  p ? new(p) ::MPMTWaveformHeader : new ::MPMTWaveformHeader;
   }
   static void *newArray_MPMTWaveformHeader(Long_t nElements, void *p) {
      return p ? new(p) ::MPMTWaveformHeader[nElements] : new ::MPMTWaveformHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_MPMTWaveformHeader(void *p) {
      delete ((::MPMTWaveformHeader*)p);
   }
   static void deleteArray_MPMTWaveformHeader(void *p) {
      delete [] ((::MPMTWaveformHeader*)p);
   }
   static void destruct_MPMTWaveformHeader(void *p) {
      typedef ::MPMTWaveformHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MPMTWaveformHeader

namespace ROOT {
   // Wrappers around operator new
   static void *new_P_MPMTWaveformHeader(void *p) {
      return  p ? new(p) ::P_MPMTWaveformHeader : new ::P_MPMTWaveformHeader;
   }
   static void *newArray_P_MPMTWaveformHeader(Long_t nElements, void *p) {
      return p ? new(p) ::P_MPMTWaveformHeader[nElements] : new ::P_MPMTWaveformHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_P_MPMTWaveformHeader(void *p) {
      delete ((::P_MPMTWaveformHeader*)p);
   }
   static void deleteArray_P_MPMTWaveformHeader(void *p) {
      delete [] ((::P_MPMTWaveformHeader*)p);
   }
   static void destruct_P_MPMTWaveformHeader(void *p) {
      typedef ::P_MPMTWaveformHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::P_MPMTWaveformHeader

namespace ROOT {
   // Wrappers around operator new
   static void *new_MPMTHit(void *p) {
      return  p ? new(p) ::MPMTHit : new ::MPMTHit;
   }
   static void *newArray_MPMTHit(Long_t nElements, void *p) {
      return p ? new(p) ::MPMTHit[nElements] : new ::MPMTHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_MPMTHit(void *p) {
      delete ((::MPMTHit*)p);
   }
   static void deleteArray_MPMTHit(void *p) {
      delete [] ((::MPMTHit*)p);
   }
   static void destruct_MPMTHit(void *p) {
      typedef ::MPMTHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MPMTHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_P_MPMTHit(void *p) {
      return  p ? new(p) ::P_MPMTHit : new ::P_MPMTHit;
   }
   static void *newArray_P_MPMTHit(Long_t nElements, void *p) {
      return p ? new(p) ::P_MPMTHit[nElements] : new ::P_MPMTHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_P_MPMTHit(void *p) {
      delete ((::P_MPMTHit*)p);
   }
   static void deleteArray_P_MPMTHit(void *p) {
      delete [] ((::P_MPMTHit*)p);
   }
   static void destruct_P_MPMTHit(void *p) {
      typedef ::P_MPMTHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::P_MPMTHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_PReadoutWindow(void *p) {
      return  p ? new(p) ::PReadoutWindow : new ::PReadoutWindow;
   }
   static void *newArray_PReadoutWindow(Long_t nElements, void *p) {
      return p ? new(p) ::PReadoutWindow[nElements] : new ::PReadoutWindow[nElements];
   }
   // Wrapper around operator delete
   static void delete_PReadoutWindow(void *p) {
      delete ((::PReadoutWindow*)p);
   }
   static void deleteArray_PReadoutWindow(void *p) {
      delete [] ((::PReadoutWindow*)p);
   }
   static void destruct_PReadoutWindow(void *p) {
      typedef ::PReadoutWindow current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PReadoutWindow

namespace ROOT {
   // Wrappers around operator new
   static void *new_MPMTWaveformSamples(void *p) {
      return  p ? new(p) ::MPMTWaveformSamples : new ::MPMTWaveformSamples;
   }
   static void *newArray_MPMTWaveformSamples(Long_t nElements, void *p) {
      return p ? new(p) ::MPMTWaveformSamples[nElements] : new ::MPMTWaveformSamples[nElements];
   }
   // Wrapper around operator delete
   static void delete_MPMTWaveformSamples(void *p) {
      delete ((::MPMTWaveformSamples*)p);
   }
   static void deleteArray_MPMTWaveformSamples(void *p) {
      delete [] ((::MPMTWaveformSamples*)p);
   }
   static void destruct_MPMTWaveformSamples(void *p) {
      typedef ::MPMTWaveformSamples current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MPMTWaveformSamples

namespace ROOT {
   // Wrappers around operator new
   static void *new_MPMTMessageData(void *p) {
      return  p ? new(p) ::MPMTMessageData : new ::MPMTMessageData;
   }
   static void *newArray_MPMTMessageData(Long_t nElements, void *p) {
      return p ? new(p) ::MPMTMessageData[nElements] : new ::MPMTMessageData[nElements];
   }
   // Wrapper around operator delete
   static void delete_MPMTMessageData(void *p) {
      delete ((::MPMTMessageData*)p);
   }
   static void deleteArray_MPMTMessageData(void *p) {
      delete [] ((::MPMTMessageData*)p);
   }
   static void destruct_MPMTMessageData(void *p) {
      typedef ::MPMTMessageData current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MPMTMessageData

namespace ROOT {
   static TClass *vectorlETriggerInfogR_Dictionary();
   static void vectorlETriggerInfogR_TClassManip(TClass*);
   static void *new_vectorlETriggerInfogR(void *p = 0);
   static void *newArray_vectorlETriggerInfogR(Long_t size, void *p);
   static void delete_vectorlETriggerInfogR(void *p);
   static void deleteArray_vectorlETriggerInfogR(void *p);
   static void destruct_vectorlETriggerInfogR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TriggerInfo>*)
   {
      vector<TriggerInfo> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TriggerInfo>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TriggerInfo>", -2, "vector", 389,
                  typeid(vector<TriggerInfo>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETriggerInfogR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TriggerInfo>) );
      instance.SetNew(&new_vectorlETriggerInfogR);
      instance.SetNewArray(&newArray_vectorlETriggerInfogR);
      instance.SetDelete(&delete_vectorlETriggerInfogR);
      instance.SetDeleteArray(&deleteArray_vectorlETriggerInfogR);
      instance.SetDestructor(&destruct_vectorlETriggerInfogR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TriggerInfo> >()));

      ::ROOT::AddClassAlternate("vector<TriggerInfo>","std::vector<TriggerInfo, std::allocator<TriggerInfo> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TriggerInfo>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETriggerInfogR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TriggerInfo>*)0x0)->GetClass();
      vectorlETriggerInfogR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETriggerInfogR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETriggerInfogR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TriggerInfo> : new vector<TriggerInfo>;
   }
   static void *newArray_vectorlETriggerInfogR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TriggerInfo>[nElements] : new vector<TriggerInfo>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETriggerInfogR(void *p) {
      delete ((vector<TriggerInfo>*)p);
   }
   static void deleteArray_vectorlETriggerInfogR(void *p) {
      delete [] ((vector<TriggerInfo>*)p);
   }
   static void destruct_vectorlETriggerInfogR(void *p) {
      typedef vector<TriggerInfo> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TriggerInfo>

namespace ROOT {
   static TClass *vectorlETriggerInfomUgR_Dictionary();
   static void vectorlETriggerInfomUgR_TClassManip(TClass*);
   static void *new_vectorlETriggerInfomUgR(void *p = 0);
   static void *newArray_vectorlETriggerInfomUgR(Long_t size, void *p);
   static void delete_vectorlETriggerInfomUgR(void *p);
   static void deleteArray_vectorlETriggerInfomUgR(void *p);
   static void destruct_vectorlETriggerInfomUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TriggerInfo*>*)
   {
      vector<TriggerInfo*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TriggerInfo*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TriggerInfo*>", -2, "vector", 389,
                  typeid(vector<TriggerInfo*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETriggerInfomUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TriggerInfo*>) );
      instance.SetNew(&new_vectorlETriggerInfomUgR);
      instance.SetNewArray(&newArray_vectorlETriggerInfomUgR);
      instance.SetDelete(&delete_vectorlETriggerInfomUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETriggerInfomUgR);
      instance.SetDestructor(&destruct_vectorlETriggerInfomUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TriggerInfo*> >()));

      ::ROOT::AddClassAlternate("vector<TriggerInfo*>","std::vector<TriggerInfo*, std::allocator<TriggerInfo*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TriggerInfo*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETriggerInfomUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TriggerInfo*>*)0x0)->GetClass();
      vectorlETriggerInfomUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETriggerInfomUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETriggerInfomUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TriggerInfo*> : new vector<TriggerInfo*>;
   }
   static void *newArray_vectorlETriggerInfomUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TriggerInfo*>[nElements] : new vector<TriggerInfo*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETriggerInfomUgR(void *p) {
      delete ((vector<TriggerInfo*>*)p);
   }
   static void deleteArray_vectorlETriggerInfomUgR(void *p) {
      delete [] ((vector<TriggerInfo*>*)p);
   }
   static void destruct_vectorlETriggerInfomUgR(void *p) {
      typedef vector<TriggerInfo*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TriggerInfo*>

namespace ROOT {
   static TClass *vectorlETDCHitmUgR_Dictionary();
   static void vectorlETDCHitmUgR_TClassManip(TClass*);
   static void *new_vectorlETDCHitmUgR(void *p = 0);
   static void *newArray_vectorlETDCHitmUgR(Long_t size, void *p);
   static void delete_vectorlETDCHitmUgR(void *p);
   static void deleteArray_vectorlETDCHitmUgR(void *p);
   static void destruct_vectorlETDCHitmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TDCHit*>*)
   {
      vector<TDCHit*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TDCHit*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TDCHit*>", -2, "vector", 389,
                  typeid(vector<TDCHit*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETDCHitmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TDCHit*>) );
      instance.SetNew(&new_vectorlETDCHitmUgR);
      instance.SetNewArray(&newArray_vectorlETDCHitmUgR);
      instance.SetDelete(&delete_vectorlETDCHitmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETDCHitmUgR);
      instance.SetDestructor(&destruct_vectorlETDCHitmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TDCHit*> >()));

      ::ROOT::AddClassAlternate("vector<TDCHit*>","std::vector<TDCHit*, std::allocator<TDCHit*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TDCHit*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETDCHitmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TDCHit*>*)0x0)->GetClass();
      vectorlETDCHitmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETDCHitmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETDCHitmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TDCHit*> : new vector<TDCHit*>;
   }
   static void *newArray_vectorlETDCHitmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TDCHit*>[nElements] : new vector<TDCHit*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETDCHitmUgR(void *p) {
      delete ((vector<TDCHit*>*)p);
   }
   static void deleteArray_vectorlETDCHitmUgR(void *p) {
      delete [] ((vector<TDCHit*>*)p);
   }
   static void destruct_vectorlETDCHitmUgR(void *p) {
      typedef vector<TDCHit*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TDCHit*>

namespace ROOT {
   static TClass *vectorlEQDCHitmUgR_Dictionary();
   static void vectorlEQDCHitmUgR_TClassManip(TClass*);
   static void *new_vectorlEQDCHitmUgR(void *p = 0);
   static void *newArray_vectorlEQDCHitmUgR(Long_t size, void *p);
   static void delete_vectorlEQDCHitmUgR(void *p);
   static void deleteArray_vectorlEQDCHitmUgR(void *p);
   static void destruct_vectorlEQDCHitmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<QDCHit*>*)
   {
      vector<QDCHit*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<QDCHit*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<QDCHit*>", -2, "vector", 389,
                  typeid(vector<QDCHit*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEQDCHitmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<QDCHit*>) );
      instance.SetNew(&new_vectorlEQDCHitmUgR);
      instance.SetNewArray(&newArray_vectorlEQDCHitmUgR);
      instance.SetDelete(&delete_vectorlEQDCHitmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEQDCHitmUgR);
      instance.SetDestructor(&destruct_vectorlEQDCHitmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<QDCHit*> >()));

      ::ROOT::AddClassAlternate("vector<QDCHit*>","std::vector<QDCHit*, std::allocator<QDCHit*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<QDCHit*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEQDCHitmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<QDCHit*>*)0x0)->GetClass();
      vectorlEQDCHitmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEQDCHitmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEQDCHitmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<QDCHit*> : new vector<QDCHit*>;
   }
   static void *newArray_vectorlEQDCHitmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<QDCHit*>[nElements] : new vector<QDCHit*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEQDCHitmUgR(void *p) {
      delete ((vector<QDCHit*>*)p);
   }
   static void deleteArray_vectorlEQDCHitmUgR(void *p) {
      delete [] ((vector<QDCHit*>*)p);
   }
   static void destruct_vectorlEQDCHitmUgR(void *p) {
      typedef vector<QDCHit*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<QDCHit*>

namespace ROOT {
   static TClass *vectorlEP_MPMTWaveformHeadergR_Dictionary();
   static void vectorlEP_MPMTWaveformHeadergR_TClassManip(TClass*);
   static void *new_vectorlEP_MPMTWaveformHeadergR(void *p = 0);
   static void *newArray_vectorlEP_MPMTWaveformHeadergR(Long_t size, void *p);
   static void delete_vectorlEP_MPMTWaveformHeadergR(void *p);
   static void deleteArray_vectorlEP_MPMTWaveformHeadergR(void *p);
   static void destruct_vectorlEP_MPMTWaveformHeadergR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<P_MPMTWaveformHeader>*)
   {
      vector<P_MPMTWaveformHeader> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<P_MPMTWaveformHeader>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<P_MPMTWaveformHeader>", -2, "vector", 389,
                  typeid(vector<P_MPMTWaveformHeader>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEP_MPMTWaveformHeadergR_Dictionary, isa_proxy, 4,
                  sizeof(vector<P_MPMTWaveformHeader>) );
      instance.SetNew(&new_vectorlEP_MPMTWaveformHeadergR);
      instance.SetNewArray(&newArray_vectorlEP_MPMTWaveformHeadergR);
      instance.SetDelete(&delete_vectorlEP_MPMTWaveformHeadergR);
      instance.SetDeleteArray(&deleteArray_vectorlEP_MPMTWaveformHeadergR);
      instance.SetDestructor(&destruct_vectorlEP_MPMTWaveformHeadergR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<P_MPMTWaveformHeader> >()));

      ::ROOT::AddClassAlternate("vector<P_MPMTWaveformHeader>","std::vector<P_MPMTWaveformHeader, std::allocator<P_MPMTWaveformHeader> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<P_MPMTWaveformHeader>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEP_MPMTWaveformHeadergR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<P_MPMTWaveformHeader>*)0x0)->GetClass();
      vectorlEP_MPMTWaveformHeadergR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEP_MPMTWaveformHeadergR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEP_MPMTWaveformHeadergR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTWaveformHeader> : new vector<P_MPMTWaveformHeader>;
   }
   static void *newArray_vectorlEP_MPMTWaveformHeadergR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTWaveformHeader>[nElements] : new vector<P_MPMTWaveformHeader>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEP_MPMTWaveformHeadergR(void *p) {
      delete ((vector<P_MPMTWaveformHeader>*)p);
   }
   static void deleteArray_vectorlEP_MPMTWaveformHeadergR(void *p) {
      delete [] ((vector<P_MPMTWaveformHeader>*)p);
   }
   static void destruct_vectorlEP_MPMTWaveformHeadergR(void *p) {
      typedef vector<P_MPMTWaveformHeader> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<P_MPMTWaveformHeader>

namespace ROOT {
   static TClass *vectorlEP_MPMTWaveformHeadermUgR_Dictionary();
   static void vectorlEP_MPMTWaveformHeadermUgR_TClassManip(TClass*);
   static void *new_vectorlEP_MPMTWaveformHeadermUgR(void *p = 0);
   static void *newArray_vectorlEP_MPMTWaveformHeadermUgR(Long_t size, void *p);
   static void delete_vectorlEP_MPMTWaveformHeadermUgR(void *p);
   static void deleteArray_vectorlEP_MPMTWaveformHeadermUgR(void *p);
   static void destruct_vectorlEP_MPMTWaveformHeadermUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<P_MPMTWaveformHeader*>*)
   {
      vector<P_MPMTWaveformHeader*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<P_MPMTWaveformHeader*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<P_MPMTWaveformHeader*>", -2, "vector", 389,
                  typeid(vector<P_MPMTWaveformHeader*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEP_MPMTWaveformHeadermUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<P_MPMTWaveformHeader*>) );
      instance.SetNew(&new_vectorlEP_MPMTWaveformHeadermUgR);
      instance.SetNewArray(&newArray_vectorlEP_MPMTWaveformHeadermUgR);
      instance.SetDelete(&delete_vectorlEP_MPMTWaveformHeadermUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEP_MPMTWaveformHeadermUgR);
      instance.SetDestructor(&destruct_vectorlEP_MPMTWaveformHeadermUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<P_MPMTWaveformHeader*> >()));

      ::ROOT::AddClassAlternate("vector<P_MPMTWaveformHeader*>","std::vector<P_MPMTWaveformHeader*, std::allocator<P_MPMTWaveformHeader*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<P_MPMTWaveformHeader*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEP_MPMTWaveformHeadermUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<P_MPMTWaveformHeader*>*)0x0)->GetClass();
      vectorlEP_MPMTWaveformHeadermUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEP_MPMTWaveformHeadermUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEP_MPMTWaveformHeadermUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTWaveformHeader*> : new vector<P_MPMTWaveformHeader*>;
   }
   static void *newArray_vectorlEP_MPMTWaveformHeadermUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTWaveformHeader*>[nElements] : new vector<P_MPMTWaveformHeader*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEP_MPMTWaveformHeadermUgR(void *p) {
      delete ((vector<P_MPMTWaveformHeader*>*)p);
   }
   static void deleteArray_vectorlEP_MPMTWaveformHeadermUgR(void *p) {
      delete [] ((vector<P_MPMTWaveformHeader*>*)p);
   }
   static void destruct_vectorlEP_MPMTWaveformHeadermUgR(void *p) {
      typedef vector<P_MPMTWaveformHeader*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<P_MPMTWaveformHeader*>

namespace ROOT {
   static TClass *vectorlEP_MPMTLEDmUgR_Dictionary();
   static void vectorlEP_MPMTLEDmUgR_TClassManip(TClass*);
   static void *new_vectorlEP_MPMTLEDmUgR(void *p = 0);
   static void *newArray_vectorlEP_MPMTLEDmUgR(Long_t size, void *p);
   static void delete_vectorlEP_MPMTLEDmUgR(void *p);
   static void deleteArray_vectorlEP_MPMTLEDmUgR(void *p);
   static void destruct_vectorlEP_MPMTLEDmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<P_MPMTLED*>*)
   {
      vector<P_MPMTLED*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<P_MPMTLED*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<P_MPMTLED*>", -2, "vector", 389,
                  typeid(vector<P_MPMTLED*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEP_MPMTLEDmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<P_MPMTLED*>) );
      instance.SetNew(&new_vectorlEP_MPMTLEDmUgR);
      instance.SetNewArray(&newArray_vectorlEP_MPMTLEDmUgR);
      instance.SetDelete(&delete_vectorlEP_MPMTLEDmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEP_MPMTLEDmUgR);
      instance.SetDestructor(&destruct_vectorlEP_MPMTLEDmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<P_MPMTLED*> >()));

      ::ROOT::AddClassAlternate("vector<P_MPMTLED*>","std::vector<P_MPMTLED*, std::allocator<P_MPMTLED*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<P_MPMTLED*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEP_MPMTLEDmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<P_MPMTLED*>*)0x0)->GetClass();
      vectorlEP_MPMTLEDmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEP_MPMTLEDmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEP_MPMTLEDmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTLED*> : new vector<P_MPMTLED*>;
   }
   static void *newArray_vectorlEP_MPMTLEDmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTLED*>[nElements] : new vector<P_MPMTLED*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEP_MPMTLEDmUgR(void *p) {
      delete ((vector<P_MPMTLED*>*)p);
   }
   static void deleteArray_vectorlEP_MPMTLEDmUgR(void *p) {
      delete [] ((vector<P_MPMTLED*>*)p);
   }
   static void destruct_vectorlEP_MPMTLEDmUgR(void *p) {
      typedef vector<P_MPMTLED*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<P_MPMTLED*>

namespace ROOT {
   static TClass *vectorlEP_MPMTHitgR_Dictionary();
   static void vectorlEP_MPMTHitgR_TClassManip(TClass*);
   static void *new_vectorlEP_MPMTHitgR(void *p = 0);
   static void *newArray_vectorlEP_MPMTHitgR(Long_t size, void *p);
   static void delete_vectorlEP_MPMTHitgR(void *p);
   static void deleteArray_vectorlEP_MPMTHitgR(void *p);
   static void destruct_vectorlEP_MPMTHitgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<P_MPMTHit>*)
   {
      vector<P_MPMTHit> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<P_MPMTHit>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<P_MPMTHit>", -2, "vector", 389,
                  typeid(vector<P_MPMTHit>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEP_MPMTHitgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<P_MPMTHit>) );
      instance.SetNew(&new_vectorlEP_MPMTHitgR);
      instance.SetNewArray(&newArray_vectorlEP_MPMTHitgR);
      instance.SetDelete(&delete_vectorlEP_MPMTHitgR);
      instance.SetDeleteArray(&deleteArray_vectorlEP_MPMTHitgR);
      instance.SetDestructor(&destruct_vectorlEP_MPMTHitgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<P_MPMTHit> >()));

      ::ROOT::AddClassAlternate("vector<P_MPMTHit>","std::vector<P_MPMTHit, std::allocator<P_MPMTHit> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<P_MPMTHit>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEP_MPMTHitgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<P_MPMTHit>*)0x0)->GetClass();
      vectorlEP_MPMTHitgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEP_MPMTHitgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEP_MPMTHitgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTHit> : new vector<P_MPMTHit>;
   }
   static void *newArray_vectorlEP_MPMTHitgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTHit>[nElements] : new vector<P_MPMTHit>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEP_MPMTHitgR(void *p) {
      delete ((vector<P_MPMTHit>*)p);
   }
   static void deleteArray_vectorlEP_MPMTHitgR(void *p) {
      delete [] ((vector<P_MPMTHit>*)p);
   }
   static void destruct_vectorlEP_MPMTHitgR(void *p) {
      typedef vector<P_MPMTHit> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<P_MPMTHit>

namespace ROOT {
   static TClass *vectorlEP_MPMTHitmUgR_Dictionary();
   static void vectorlEP_MPMTHitmUgR_TClassManip(TClass*);
   static void *new_vectorlEP_MPMTHitmUgR(void *p = 0);
   static void *newArray_vectorlEP_MPMTHitmUgR(Long_t size, void *p);
   static void delete_vectorlEP_MPMTHitmUgR(void *p);
   static void deleteArray_vectorlEP_MPMTHitmUgR(void *p);
   static void destruct_vectorlEP_MPMTHitmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<P_MPMTHit*>*)
   {
      vector<P_MPMTHit*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<P_MPMTHit*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<P_MPMTHit*>", -2, "vector", 389,
                  typeid(vector<P_MPMTHit*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEP_MPMTHitmUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<P_MPMTHit*>) );
      instance.SetNew(&new_vectorlEP_MPMTHitmUgR);
      instance.SetNewArray(&newArray_vectorlEP_MPMTHitmUgR);
      instance.SetDelete(&delete_vectorlEP_MPMTHitmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEP_MPMTHitmUgR);
      instance.SetDestructor(&destruct_vectorlEP_MPMTHitmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<P_MPMTHit*> >()));

      ::ROOT::AddClassAlternate("vector<P_MPMTHit*>","std::vector<P_MPMTHit*, std::allocator<P_MPMTHit*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<P_MPMTHit*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEP_MPMTHitmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<P_MPMTHit*>*)0x0)->GetClass();
      vectorlEP_MPMTHitmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEP_MPMTHitmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEP_MPMTHitmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTHit*> : new vector<P_MPMTHit*>;
   }
   static void *newArray_vectorlEP_MPMTHitmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<P_MPMTHit*>[nElements] : new vector<P_MPMTHit*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEP_MPMTHitmUgR(void *p) {
      delete ((vector<P_MPMTHit*>*)p);
   }
   static void deleteArray_vectorlEP_MPMTHitmUgR(void *p) {
      delete [] ((vector<P_MPMTHit*>*)p);
   }
   static void destruct_vectorlEP_MPMTHitmUgR(void *p) {
      typedef vector<P_MPMTHit*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<P_MPMTHit*>

namespace ROOT {
   static TClass *vectorlEMPMTWaveformSamplesgR_Dictionary();
   static void vectorlEMPMTWaveformSamplesgR_TClassManip(TClass*);
   static void *new_vectorlEMPMTWaveformSamplesgR(void *p = 0);
   static void *newArray_vectorlEMPMTWaveformSamplesgR(Long_t size, void *p);
   static void delete_vectorlEMPMTWaveformSamplesgR(void *p);
   static void deleteArray_vectorlEMPMTWaveformSamplesgR(void *p);
   static void destruct_vectorlEMPMTWaveformSamplesgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<MPMTWaveformSamples>*)
   {
      vector<MPMTWaveformSamples> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<MPMTWaveformSamples>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<MPMTWaveformSamples>", -2, "vector", 389,
                  typeid(vector<MPMTWaveformSamples>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEMPMTWaveformSamplesgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<MPMTWaveformSamples>) );
      instance.SetNew(&new_vectorlEMPMTWaveformSamplesgR);
      instance.SetNewArray(&newArray_vectorlEMPMTWaveformSamplesgR);
      instance.SetDelete(&delete_vectorlEMPMTWaveformSamplesgR);
      instance.SetDeleteArray(&deleteArray_vectorlEMPMTWaveformSamplesgR);
      instance.SetDestructor(&destruct_vectorlEMPMTWaveformSamplesgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<MPMTWaveformSamples> >()));

      ::ROOT::AddClassAlternate("vector<MPMTWaveformSamples>","std::vector<MPMTWaveformSamples, std::allocator<MPMTWaveformSamples> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<MPMTWaveformSamples>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEMPMTWaveformSamplesgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<MPMTWaveformSamples>*)0x0)->GetClass();
      vectorlEMPMTWaveformSamplesgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEMPMTWaveformSamplesgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEMPMTWaveformSamplesgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<MPMTWaveformSamples> : new vector<MPMTWaveformSamples>;
   }
   static void *newArray_vectorlEMPMTWaveformSamplesgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<MPMTWaveformSamples>[nElements] : new vector<MPMTWaveformSamples>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEMPMTWaveformSamplesgR(void *p) {
      delete ((vector<MPMTWaveformSamples>*)p);
   }
   static void deleteArray_vectorlEMPMTWaveformSamplesgR(void *p) {
      delete [] ((vector<MPMTWaveformSamples>*)p);
   }
   static void destruct_vectorlEMPMTWaveformSamplesgR(void *p) {
      typedef vector<MPMTWaveformSamples> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<MPMTWaveformSamples>

namespace ROOT {
   static TClass *vectorlEHKMPMTHitmUgR_Dictionary();
   static void vectorlEHKMPMTHitmUgR_TClassManip(TClass*);
   static void *new_vectorlEHKMPMTHitmUgR(void *p = 0);
   static void *newArray_vectorlEHKMPMTHitmUgR(Long_t size, void *p);
   static void delete_vectorlEHKMPMTHitmUgR(void *p);
   static void deleteArray_vectorlEHKMPMTHitmUgR(void *p);
   static void destruct_vectorlEHKMPMTHitmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<HKMPMTHit*>*)
   {
      vector<HKMPMTHit*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<HKMPMTHit*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<HKMPMTHit*>", -2, "vector", 389,
                  typeid(vector<HKMPMTHit*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEHKMPMTHitmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<HKMPMTHit*>) );
      instance.SetNew(&new_vectorlEHKMPMTHitmUgR);
      instance.SetNewArray(&newArray_vectorlEHKMPMTHitmUgR);
      instance.SetDelete(&delete_vectorlEHKMPMTHitmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEHKMPMTHitmUgR);
      instance.SetDestructor(&destruct_vectorlEHKMPMTHitmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<HKMPMTHit*> >()));

      ::ROOT::AddClassAlternate("vector<HKMPMTHit*>","std::vector<HKMPMTHit*, std::allocator<HKMPMTHit*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<HKMPMTHit*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEHKMPMTHitmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<HKMPMTHit*>*)0x0)->GetClass();
      vectorlEHKMPMTHitmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEHKMPMTHitmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEHKMPMTHitmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<HKMPMTHit*> : new vector<HKMPMTHit*>;
   }
   static void *newArray_vectorlEHKMPMTHitmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<HKMPMTHit*>[nElements] : new vector<HKMPMTHit*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEHKMPMTHitmUgR(void *p) {
      delete ((vector<HKMPMTHit*>*)p);
   }
   static void deleteArray_vectorlEHKMPMTHitmUgR(void *p) {
      delete [] ((vector<HKMPMTHit*>*)p);
   }
   static void destruct_vectorlEHKMPMTHitmUgR(void *p) {
      typedef vector<HKMPMTHit*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<HKMPMTHit*>

namespace {
  void TriggerDictionaryInitialization_WCTE_RootDict_Impl() {
    static const char* headers[] = {
"/root/WCTEDAQ/DataModel/DAQInfo.h",
"/root/WCTEDAQ/DataModel/ReadoutWindow.h",
"/root/WCTEDAQ/DataModel/WCTEMPMTPPS.h",
"/root/WCTEDAQ/DataModel/TDCHit.h",
"/root/WCTEDAQ/DataModel/QDCHit.h",
"/root/WCTEDAQ/DataModel/MPMTMessages.h",
"/root/WCTEDAQ/DataModel/MPMTMessageData.h",
"/root/WCTEDAQ/DataModel/MPMTWaveformSamples.h",
"/root/WCTEDAQ/DataModel/TriggerType.h",
"SerialisableObject.h",
0
    };
    static const char* includePaths[] = {
"/root/WCTEDAQ/DataModel",
"/root/WCTEDAQ/Dependencies/ToolFrameworkCore/include",
"/root/WCTEDAQ/Dependencies/root_v6.24.08/include/",
"/root/WCTEDAQ/RootDict/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "WCTE_RootDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$TriggerType.h")))  TriggerInfo;
namespace std{template <typename _Tp> class __attribute__((annotate("$clingAutoload$bits/allocator.h")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}
struct __attribute__((annotate("$clingAutoload$MPMTMessages.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  P_MPMTHit;
struct __attribute__((annotate("$clingAutoload$MPMTMessages.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  P_MPMTWaveformHeader;
struct __attribute__((annotate("$clingAutoload$MPMTWaveformSamples.h")))  MPMTWaveformSamples;
class __attribute__((annotate("$clingAutoload$SerialisableObject.h")))  SerialisableObject;
class __attribute__((annotate("$clingAutoload$WCTEMPMTPPS.h")))  __attribute__((annotate("$clingAutoload$TriggerType.h")))  WCTEMPMTPPS;
struct __attribute__((annotate("$clingAutoload$TriggerType.h")))  MPMTLED;
struct __attribute__((annotate("$clingAutoload$TriggerType.h")))  P_MPMTLED;
class __attribute__((annotate("$clingAutoload$DAQInfo.h")))  DAQInfo;
class __attribute__((annotate("$clingAutoload$TDCHit.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  TDCHit;
class __attribute__((annotate("$clingAutoload$QDCHit.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  QDCHit;
struct __attribute__((annotate("$clingAutoload$MPMTMessages.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  MPMTWaveformHeader;
struct __attribute__((annotate("$clingAutoload$MPMTMessages.h")))  __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  MPMTHit;
class __attribute__((annotate("$clingAutoload$ReadoutWindow.h")))  PReadoutWindow;
struct __attribute__((annotate("$clingAutoload$MPMTMessageData.h")))  MPMTMessageData;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "WCTE_RootDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "/root/WCTEDAQ/DataModel/DAQInfo.h"
#include "/root/WCTEDAQ/DataModel/ReadoutWindow.h"
#include "/root/WCTEDAQ/DataModel/WCTEMPMTPPS.h"
#include "/root/WCTEDAQ/DataModel/TDCHit.h"
#include "/root/WCTEDAQ/DataModel/QDCHit.h"
#include "/root/WCTEDAQ/DataModel/MPMTMessages.h"
#include "/root/WCTEDAQ/DataModel/MPMTMessageData.h"
#include "/root/WCTEDAQ/DataModel/MPMTWaveformSamples.h"
#include "/root/WCTEDAQ/DataModel/TriggerType.h"
#include "SerialisableObject.h"
#include <vector>
#include "SerialisableObject.h"
#include "TriggerType.h"
#include "DAQInfo.h"
#include "ReadoutWindow.h"
#include "MPMTMessages.h"
#include "MPMTWaveformSamples.h"
#include "MPMTMessageData.h"
#include "WCTEMPMTPPS.h"
#include "TDCHit.h"
#include "QDCHit.h"
#include "BitFunctions.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;
#pragma link C++ class SerialisableObject-;
#pragma link C++ class DAQInfo+;
#pragma link C++ class MPMTLED+;
#pragma link C++ class P_MPMTLED+;
#pragma link C++ class MPMTHit+;
#pragma link C++ class WCTEMPMTPPS+;
#pragma link C++ class TDCHit+;
#pragma link C++ class QDCHit+;
#pragma link C++ class P_MPMTHit+;
#pragma link C++ class MPMTWaveformHeader+;
#pragma link C++ class P_MPMTWaveformHeader+;
#pragma link C++ class MPMTWaveformSamples+;
#pragma link C++ class MPMTMessageData+;
#pragma link C++ class TriggerInfo+;
#pragma link C++ class vector<TriggerInfo>+;
#pragma link C++ class vector<TriggerInfo*>+;
#pragma link C++ class vector<P_MPMTHit>+;
#pragma link C++ class vector<P_MPMTHit*>+;
#pragma link C++ class vector<P_MPMTWaveformHeader>+;
#pragma link C++ class vector<P_MPMTWaveformHeader*>+;
#pragma link C++ class vector<MPMTWaveformSamples>+;
#pragma link C++ class PReadoutWindow+;

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"DAQInfo", payloadCode, "@",
"MPMTHit", payloadCode, "@",
"MPMTLED", payloadCode, "@",
"MPMTMessageData", payloadCode, "@",
"MPMTWaveformHeader", payloadCode, "@",
"MPMTWaveformSamples", payloadCode, "@",
"PReadoutWindow", payloadCode, "@",
"P_MPMTHit", payloadCode, "@",
"P_MPMTLED", payloadCode, "@",
"P_MPMTWaveformHeader", payloadCode, "@",
"QDCHit", payloadCode, "@",
"SerialisableObject", payloadCode, "@",
"TDCHit", payloadCode, "@",
"TriggerInfo", payloadCode, "@",
"WCTEMPMTPPS", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("WCTE_RootDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_WCTE_RootDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_WCTE_RootDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_WCTE_RootDict() {
  TriggerDictionaryInitialization_WCTE_RootDict_Impl();
}
