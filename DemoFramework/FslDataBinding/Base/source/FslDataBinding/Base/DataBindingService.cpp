#define LOCAL_SANITY_CHECK
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDataBinding/Base/Bind/AConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/AMultiConverterBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Internal/IDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ObserverDependencyPropertyMethods.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <utility>

#ifndef LOCAL_SANITY_CHECK
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_CHECK() \
  {                             \
  }
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_DO_SANITY_CHECK() assert(SanityCheck());
#endif

namespace Fsl::DataBinding
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t MaxExecuteLoopCount = 1024;
    }

    struct HandleArrayVector
    {
      std::array<DataBindingInstanceHandle, Internal::DbsConstants::MaxMultiBindSize> Handles;
      uint32_t Count{};

      void AddHandle(const DataBindingInstanceHandle handle) noexcept
      {
        assert(Count < Handles.size());
        Handles[Count] = handle;
        ++Count;
      }

      ReadOnlySpan<DataBindingInstanceHandle> SourceHandles() const noexcept
      {
        return ReadOnlySpanUtil::AsSpan(Handles, Count, OptimizationCheckFlag::NoCheck);
      }
    };


    Internal::InstanceState::Flags ToFlags(const DataSourceFlags srcFlags) noexcept
    {
      Internal::InstanceState::Flags dstFlags = Internal::InstanceState::Flags::NoFlags;

      if (DataSourceFlagsUtil::IsEnabled(srcFlags, DataSourceFlags::Observable))
      {
        dstFlags = dstFlags | Internal::InstanceState::Flags::Observable;
      }
      return dstFlags;
    }

    //! Erase the first instance of the given handle
    template <std::size_t TIndex, std::size_t TArraySize>
    bool EraseFirst(TightHostedVector<DataBindingInstanceHandle, TArraySize>& rHandles, DataBindingInstanceHandle handle) noexcept
    {
      static_assert(TIndex < TArraySize, "Index out of bounds");

      auto itr = rHandles.Begin(TIndex);
      const auto itrEnd = rHandles.End(TIndex);
      while (itr != itrEnd)
      {
        if (*itr == handle)
        {
          rHandles.Erase(TIndex, itr);
          return true;
        }
        ++itr;
      }
      return false;
    }

    // This will not add or remove elements from rInstances so any references or pointers into it will remain unchanged
    bool ClearSourceBindings(HandleVector<Internal::ServiceBindingRecord>& rInstances, const DataBindingInstanceHandle hTarget) noexcept
    {
      Internal::ServiceBindingRecord& rTargetRecord = rInstances.FastGet(hTarget.Value);

      const ReadOnlySpan<DataBindingInstanceHandle> sourceHandles = rTargetRecord.SourceHandles();
      FSLLOG3_VERBOSE4("Clearing source bindings from {}", hTarget.Value);
      for (DataBindingInstanceHandle srcHandle : sourceHandles)
      {
        FSLLOG3_VERBOSE4("- source {}", srcHandle.Value);

        assert(srcHandle != hTarget);
        // Lookup the source
        auto& rOldSource = rInstances.FastGet(srcHandle.Value);
        // then find and remove the key from the source targets vector
        EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(rOldSource.Handles, hTarget);
      }
      const bool changed = !sourceHandles.empty();

      rTargetRecord.ClearSourceHandles();
      return changed;
    }


    bool IsSourceBindingsBeingChanged(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                      const Binding& binding) noexcept
    {
      auto newSourceHandles = binding.SourceHandlesAsSpan();
      auto oldSourceHandles = instances.FastGet(hTarget.Value).SourceHandles();
      return newSourceHandles != oldSourceHandles;
    }


    void VerifyInstancesAreAlive(const HandleVector<Internal::ServiceBindingRecord>& instances, const ReadOnlySpan<DataBindingInstanceHandle> handles)
    {
      for (auto handle : handles)
      {
        const auto& sourceInstance = instances.Get(handle.Value);
        if (sourceInstance.Instance.GetState() != DataBindingInstanceState::Alive)
        {
          throw DeadInstanceException(fmt::format("source '{}' must be alive", handle.Value));
        }
      }
    }

    void ValidateBindToDependencyObject(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                        const Binding& binding)
    {
      FSL_PARAM_NOT_USED(instances);
      FSL_PARAM_NOT_USED(hTarget);
      FSL_PARAM_NOT_USED(binding);
      throw BindingIncompatiblePropertiesException("Bind target can not be a DependencyObject");
    }


    void ValidateBindToDataSourceObject(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                        const Binding& binding)
    {
      FSL_PARAM_NOT_USED(instances);
      FSL_PARAM_NOT_USED(hTarget);
      FSL_PARAM_NOT_USED(binding);
      throw BindingIncompatiblePropertiesException("Bind target can not be a DataSourceObject");
    }


    void ValidateBindToDependencyObserverProperty(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                  const DataBindingInstanceHandle hTarget, const Binding& binding)

    {
      // Binds to DependencyObserverProperty must have exactly one source which is a DataSourceObject
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingErrorException("ObserverProperty binding should always have a source count of 1");
      }
      if (instances.FastGet(sourceHandlesSpan.front().Value).Instance.GetType() != DataBindingInstanceType::DataSourceObject)
      {
        throw BindingIncompatiblePropertiesException("ObserverProperty binding sources should always be of type 'DataSourceObject'");
      }
      if (binding.ComplexBinding())
      {
        throw BindingErrorException("ObserverProperty binding should not have a ComplexBinding object");
      }
      if (!instances.FastGet(hTarget.Value).Methods)
      {
        throw InternalErrorException("No methods associated");
      }
      // We have a valid bind observer property bind
    }


    void ValidateTypeAndBindingCompatibilitySet(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingUnsupportedException(fmt::format(
          "a basic bind between a target and source can only contain exactly one source. But {} sources was supplised", sourceHandlesSpan.size()));
      }

      // Verify that the types match
      if (instances.FastGet(sourceHandlesSpan.front().Value).Methods->GetType() != targetInstance.GetType())
      {
        // The types did not match
        throw BindingIncompatibleTypesException(
          "target and source type must match or a ConversionBinding or MultiConversionBinding must be supplied");
      }
    }

    void ValidateTypeAndBindingCompatibilityConvert(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                    const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      if (sourceHandlesSpan.size() != 1u)
      {
        throw BindingUnsupportedException("AConverterBinding takes exactly one source as input!");
      }

      const Internal::ServiceBindingRecord& sourceRecord = instances.FastGet(sourceHandlesSpan.front().Value);

      const std::type_index propertySourceType = sourceRecord.Methods->GetType();
      const std::type_index propertyTargetType = targetInstance.GetType();

      // Warning removed as there are good reasons to do this (forexample to do a calculation on the value)
      // FSLLOG3_WARNING_IF(propertySourceType == propertyTargetType, "A conversion binding is not necessary as the types are compatible");

      const auto* const pConverter = dynamic_cast<const AConverterBinding*>(binding.ComplexBinding().get());
      if (pConverter == nullptr)
      {
        throw BindingIncompatibleTypesException("binding was not of the promised type!");
      }

      const std::type_index converterSourceType = pConverter->GetSourceType();
      if (propertySourceType != converterSourceType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion source binding '{}' is not compatible with the expected source type of '{}'",
                                                            converterSourceType.name(), propertySourceType.name()));
      }
      const std::type_index converterTargetType = pConverter->GetTargetType();
      if (propertyTargetType != converterTargetType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion target binding '{}' is not compatible with the expected target type of '{}'",
                                                            converterTargetType.name(), propertyTargetType.name()));
      }
    }

    void ValidateTypeAndBindingCompatibilityMultiConvert(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                         const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      const auto* const pConverter = dynamic_cast<const AMultiConverterBinding*>(binding.ComplexBinding().get());
      if (pConverter == nullptr)
      {
        throw BindingIncompatibleTypesException("binding was not of the promised type!");
      }

      {    // Verify the the converter matches the supplied sources
        const ReadOnlySpan<PropertyTypeInfo> converterSourceTypesSpan = pConverter->GetSourceTypes();
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        if (converterSourceTypesSpan.size() != sourceHandlesSpan.size())
        {
          throw BindingUnsupportedException(fmt::format("AMultiConverterBinding was expecting {} bindings but only {} were provided",
                                                        converterSourceTypesSpan.size(), sourceHandlesSpan.size()));
        }
        // This has been verified earlier
        assert(!sourceHandlesSpan.empty());

        // Run through the source bindings and verify the types match the expected converter types
        for (std::size_t sourceIndex = 0; sourceIndex < sourceHandlesSpan.size(); ++sourceIndex)
        {
          const auto hSource = sourceHandlesSpan[sourceIndex];

          const Internal::ServiceBindingRecord& sourceRecord = instances.FastGet(hSource.Value);

          const std::type_index propertySourceType = sourceRecord.Methods->GetType();
          if (propertySourceType != converterSourceTypesSpan[sourceIndex].TypeIndex)
          {
            throw BindingIncompatibleTypesException(
              fmt::format("Conversion source binding '{}' is not compatible with the expected source type of '{}'",
                          converterSourceTypesSpan[sourceIndex].TypeIndex.name(), propertySourceType.name()));
          }
        }
      }

      const std::type_index propertyTargetType = targetInstance.GetType();
      const std::type_index converterTargetType = pConverter->GetTargetType();
      if (propertyTargetType != converterTargetType)
      {
        throw BindingIncompatibleTypesException(fmt::format("Conversion target binding '{}' is not compatible with the expected target type of '{}'",
                                                            converterTargetType.name(), propertyTargetType.name()));
      }
    }

    void ValidateTypeAndBindingCompatibility(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                             const Internal::IPropertyMethods& targetInstance, const Binding& binding)
    {
      if (!binding.ComplexBinding())
      {
        ValidateTypeAndBindingCompatibilitySet(instances, targetInstance, binding);
      }
      else
      {
        const BindingType complexBindingType = binding.ComplexBinding()->GetBindingType();
        switch (complexBindingType)
        {
        case BindingType::AConverterBinding:
          ValidateTypeAndBindingCompatibilityConvert(instances, targetInstance, binding);
          break;
        case BindingType::AMultiConverterBinding:
          ValidateTypeAndBindingCompatibilityMultiConvert(instances, targetInstance, binding);
          break;
        default:
          throw BindingUnsupportedException(fmt::format("Unsupported binding type: {}", static_cast<int32_t>(complexBindingType)));
        }
      }
    }

    void ValidateBindToDependencyProperty(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget,
                                          const Binding& binding)
    {
      const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
      // We already verified that this is not empty earlier
      assert(!sourceHandlesSpan.empty());


      // DependencyProperty require that all sources are DependencyProperty's and they must have methods attached
      for (const auto hSource : sourceHandlesSpan)
      {
        const Internal::ServiceBindingRecord& sourcRecord = instances.FastGet(hSource.Value);
        if (sourcRecord.Instance.GetType() != DataBindingInstanceType::DependencyProperty)
        {
          throw BindingIncompatiblePropertiesException("hSource must be a DependencyProperty");
        }
        if (!sourcRecord.Methods)
        {
          // Source must have methods associated (that should have been checked at creation time)
          throw InternalErrorException("No methods associated");
        }
      }

      const Internal::ServiceBindingRecord& targetRecord = instances.FastGet(hTarget.Value);
      if (!targetRecord.Methods)
      {
        // Target must have methods associated (that should have been checked at creation time)
        throw InternalErrorException("No methods associated");
      }

      ValidateTypeAndBindingCompatibility(instances, *targetRecord.Methods, binding);
    }


    void ValidateBind(const HandleVector<Internal::ServiceBindingRecord>& instances, const DataBindingInstanceHandle hTarget, const Binding& binding)
    {
      // Check that the target is alive
      if (instances.Get(hTarget.Value).Instance.GetState() != DataBindingInstanceState::Alive)
      {
        throw DeadInstanceException("hTarget must be alive");
      }

      {    // Verify that there are at least one source binding and that all source bindings are alive
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        const auto sourceHandleCount = sourceHandlesSpan.size();
        if (sourceHandleCount <= 0u)
        {
          throw BindingUnsupportedException("There should always be at least one source in a binding");
        }
        if (sourceHandleCount > Internal::DbsConstants::MaxMultiBindSize)
        {
          throw BindingUnsupportedException(
            fmt::format("There can not be more than {} source bind entries. Got: {}", Internal::DbsConstants::MaxMultiBindSize, sourceHandleCount));
        }

        // Verify that all the source instances are alive
        VerifyInstancesAreAlive(instances, sourceHandlesSpan);
      }
      // At this point all handles we got from outside has been 'verified' so we can use 'FastGet' operations to retrieve records from now on

      const DataBindingInstanceType targetInstanceType = instances.FastGet(hTarget.Value).Instance.GetType();
      switch (targetInstanceType)
      {
      case DataBindingInstanceType::DependencyObject:
        ValidateBindToDependencyObject(instances, hTarget, binding);
        break;
      case DataBindingInstanceType::DependencyProperty:
        ValidateBindToDependencyProperty(instances, hTarget, binding);
        break;
      case DataBindingInstanceType::DependencyObserverProperty:
        ValidateBindToDependencyObserverProperty(instances, hTarget, binding);
        break;
      case DataBindingInstanceType::DataSourceObject:
        ValidateBindToDataSourceObject(instances, hTarget, binding);
        break;
      default:
        throw BindingUnsupportedException(fmt::format("Unsupported DataBindingInstanceType: {}", static_cast<int32_t>(targetInstanceType)));
      }
    }

    Internal::PropertySetResult ExecuteDependencyPropertyGetSetMultiConverterBinding(const HandleVector<Internal::ServiceBindingRecord>& instances,
                                                                                     const Internal::ServiceBindingRecord& target)
    {
      auto* const pMultiConverter = dynamic_cast<AMultiConverterBinding*>(target.SourceUserBinding().get());
      if (pMultiConverter != nullptr)
      {
        std::array<Internal::PropertyGetInfo, Internal::DbsConstants::MaxMultiBindSize> getters;
        auto sourceHandles = target.SourceHandles();
        assert(sourceHandles.size() <= getters.size());
        for (std::size_t i = 0; i < sourceHandles.size(); ++i)
        {
          const Internal::ServiceBindingRecord* const pSourceRecord = instances.TryGet(sourceHandles[i].Value);
          if (pSourceRecord == nullptr)
          {
            FSLLOG3_DEBUG_WARNING("Failed to acqurie source binding record");
            return Internal::PropertySetResult::UnsupportedGetType;
          }
          getters[i] = Internal::PropertyGetInfo(pSourceRecord->Instance.GetPropertyMethodsImplType(), pSourceRecord->Methods.get());
        }
        return pMultiConverter->Convert(target.Instance.GetPropertyMethodsImplType(), target.Methods.get(),
                                        ReadOnlySpanUtil::AsSpan(getters, sourceHandles.size(), OptimizationCheckFlag::NoCheck));
      }
      return Internal::PropertySetResult::UnsupportedBindingType;
    }
  }

  DataBindingService::~DataBindingService()
  {
    MarkShutdownIntend();
  }

  void DataBindingService::MarkShutdownIntend() noexcept
  {
    DestroyScheduledNow();
    if (!m_instances.Empty())
    {
      FSLLOG3_ERROR("there are {} data binding instances that where not properly destroyed", m_instances.Count());
    }
  }

  DataBindingInstanceHandle DataBindingService::CreateDependencyObject()
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObject: Can not be called from this context");
    }

    EnsureDestroyCapacity();
    auto handle = m_instances.Add(Internal::ServiceBindingRecord(DataBindingInstanceType::DependencyObject, Internal::InstanceState::Flags::NoFlags));
    return DataBindingInstanceHandle(handle);
  }


  DataBindingInstanceHandle DataBindingService::CreateDataSourceObject(const DataSourceFlags flags)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDataSourceObject: Can not be called from this context");
    }

    EnsureDestroyCapacity();
    auto handle = m_instances.Add(Internal::ServiceBindingRecord(DataBindingInstanceType::DataSourceObject, ToFlags(flags)));
    return DataBindingInstanceHandle(handle);
  }

  DataBindingInstanceHandle DataBindingService::CreateDependencyObjectProperty(DataBindingInstanceHandle hDependencyObject,
                                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                                               std::unique_ptr<Internal::IDependencyPropertyMethods> methods)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObjectProperty: Can not be called from this context");
    }

    // Properties by default are observable (so they can send change notifications)
    const auto flags = Internal::InstanceState::Flags::Observable;
    return DoCreateDependencyObjectProperty(hDependencyObject, propertyDefinition, std::move(methods), DataBindingInstanceType::DependencyProperty,
                                            flags);
  }

  DataBindingInstanceHandle
    DataBindingService::CreateDependencyObjectObserverProperty(const DataBindingInstanceHandle hOwner,
                                                               const DependencyPropertyDefinition& propertyDefinition,
                                                               std::unique_ptr<Internal::ObserverDependencyPropertyMethods> methods)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      throw UsageErrorException("CreateDependencyObjectObserverProperty: Can not be called from this context");
    }

    // Observer properties are by default not observable (since they are used to receive)
    const auto flags = Internal::InstanceState::Flags::NoFlags;
    return DoCreateDependencyObjectProperty(hOwner, propertyDefinition, std::move(methods), DataBindingInstanceType::DependencyObserverProperty,
                                            flags);
  }


  bool DataBindingService::DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept
  {
    assert(!m_instances.IsValidHandle(hInstance.Value) ||
           (m_instances.FastGet(hInstance.Value).Instance.GetType() == DataBindingInstanceType::DependencyProperty ||
            m_instances.FastGet(hInstance.Value).Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty));
    return DestroyInstance(hInstance);
  }


  bool DataBindingService::DestroyInstance(const DataBindingInstanceHandle hInstance) noexcept
  {
    LOCAL_DO_SANITY_CHECK();

    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks);

    Internal::ServiceBindingRecord* pRecord = m_instances.TryGet(hInstance.Value);
    const bool wasKnownAndAlive = pRecord != nullptr && pRecord->Instance.GetState() == DataBindingInstanceState::Alive;
    if (wasKnownAndAlive)
    {
      // Mark the record as destroyed and schedule it for destruction
      pRecord->Instance.SetDataBindingInstanceState(DataBindingInstanceState::DestroyScheduled);
      pRecord->Methods.reset();    // Clear the methods to ensure we don't call a invalid pointer
      for (const auto hProperty : pRecord->Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Properties))
      {
        Internal::ServiceBindingRecord& rPropertyRecord = m_instances.FastGet(hProperty.Value);
        rPropertyRecord.Instance.SetDataBindingInstanceState(DataBindingInstanceState::DestroyScheduled);
        rPropertyRecord.Methods.reset();    // Clear the methods to ensure we don't call a invalid pointer
      }
      assert(m_scheduledForDestroy.capacity() > m_scheduledForDestroy.size());
      try
      {
        // Finally schedule the instance for destruction
        m_scheduledForDestroy.push_back(hInstance);
      }
      catch (const std::exception&)
      {
        FSLLOG3_ERROR("push failed, this should never happen as we always ensure that capacity is can contain all possible instances");
        std::abort();
      }
    }
    LOCAL_DO_SANITY_CHECK();
    return wasKnownAndAlive;
  }

  bool DataBindingService::ClearBinding(const DataBindingInstanceHandle hTarget)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks &&
        m_callContext.State != CallContextState::ExecutingChanges)
    {
      throw UsageErrorException("ClearBinding: Can not be called from this context");
    }
    if (m_instances.Get(hTarget.Value).Instance.GetState() != DataBindingInstanceState::Alive)
    {
      throw DeadInstanceException("hSource must be alive");
    }
    return ClearSourceBindings(m_instances, hTarget);
  }

  bool DataBindingService::SetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      const Internal::ServiceBindingRecord& targetInstance = m_instances.Get(hTarget.Value);
      if (targetInstance.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty)
      {
        // Validate the bind right away before we defer it
        ValidateBind(m_instances, hTarget, binding);
        // DependencyObserverProperty has no complex bindings and exactly one source
        assert(!binding.ComplexBinding());
        const auto sourceHandlesSpan = binding.SourceHandlesAsSpan();
        assert(sourceHandlesSpan.size() == 1u);
        m_pendingBindings.emplace(hTarget, sourceHandlesSpan.front());
        return true;
      }
      throw UsageErrorException("SetBinding: Can not be called from this context");
    }
    return DoSetBinding(hTarget, binding);
  }


  bool DataBindingService::Changed(const DataBindingInstanceHandle hInstance)
  {
    if (m_callContext.State != CallContextState::Idle && m_callContext.State != CallContextState::ExecutingObserverCallbacks)
    {
      if (m_callContext.State == CallContextState::ExecutingChanges && m_callContext.Handle == hInstance)
      {
        // We ignore 'changed' calls from the specific instance that we are executing a callback to
        return true;
      }
      throw UsageErrorException("Changed: Can not be called from this context");
    }
    Internal::ServiceBindingRecord& rChangedInstance = m_instances.Get(hInstance.Value);
    return ScheduleChanged(hInstance, rChangedInstance, false);
  }


  bool DataBindingService::IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept
  {
    bool isReadOnly = false;
    if (m_callContext.State != CallContextState::ExecutingChanges)
    {
      const Internal::ServiceBindingRecord* const pRecord = m_instances.TryGet(hInstance.Value);
      isReadOnly = pRecord == nullptr || pRecord->HasValidSourceHandles();
    }
    return isReadOnly;
  }


  void DataBindingService::ExecuteChanges()
  {
    if (m_callContext.State != CallContextState::Idle)
    {
      throw UsageErrorException("ExecuteChanges: Can not be called from this context");
    }
    uint32_t changeLoopCounter = 0;
    do
    {
      // Since we are single threaded and destroy the instance id's here
      // It means that the 'deferred' changes to destroyed instances will fail to acquire their instance and thereby be ignored.
      DestroyScheduledNow();

      // Execute all pending changes
      ExecutePendingChangesNow();
      // Execute all pending observer bindings now
      ExecutePendingBindingsNow();

      ExecuteObserverCallbacksNow();
      ++changeLoopCounter;
    } while (!m_changes.empty() && changeLoopCounter < LocalConfig::MaxExecuteLoopCount);
    FSLLOG3_WARNING_IF(changeLoopCounter >= LocalConfig::MaxExecuteLoopCount, "ExecuteChanges max allowed loops reached");
  }


  bool DataBindingService::DoSetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding)
  {
    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks ||
           m_callContext.State == CallContextState::ExecutePendingBindings);
    LOCAL_DO_SANITY_CHECK();
    if (!binding.HasValidSourceHandles())
    {
      return ClearBinding(hTarget);
    }
    try
    {
      if (binding.ContainsSource(hTarget))
      {
        throw CyclicBindingException("Circular dependency found (cant bind to itself)");
      }
      ValidateBind(m_instances, hTarget, binding);
    }
    catch (const std::exception&)
    {
      ClearSourceBindings(m_instances, hTarget);
      throw;
    }

    const bool changed = IsSourceBindingsBeingChanged(m_instances, hTarget, binding);
    if (changed)
    {
      // the binding was changed, so delete the old binding (if it exist)
      ClearSourceBindings(m_instances, hTarget);
    }

    LOCAL_DO_SANITY_CHECK();

    // check if the binding is being modified
    if (changed)
    {
      HandleArrayVector inProgressSourceBindings;
      try
      {
        for (const auto hSource : binding.SourceHandlesAsSpan())
        {
          inProgressSourceBindings.AddHandle(hSource);

          // Check if the source would create a cyclic dependency
          CheckForCyclicDependencies(hTarget, hSource);

          // Link the source to the target
          // - We do the push first so that if it throws 'the new bind' will be fully reversed.
          //   But we do lose any previous binding that might have been removed already

          m_instances.FastGet(hSource.Value).Handles.PushBack(Internal::ServicePropertyVectorIndex::Targets, hTarget);
        }

        // All cyclic dependency checks passed and we updated all the sources with hTarget as a target
        // So its time to update the target record with the new source
        Internal::ServiceBindingRecord& rTargetInstance = m_instances.FastGet(hTarget.Value);
        rTargetInstance.SetSource(binding);

        LOCAL_DO_SANITY_CHECK();

        // We only allow skipping during
        assert(m_callContext.State != CallContextState::ExecutePendingBindings ||
               rTargetInstance.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty);

        if (m_callContext.State != CallContextState::ExecutePendingBindings)
        {
          // Mark as source as changed as a simple but slightly expensive way to populate the target
          for (const auto hSource : binding.SourceHandlesAsSpan())
          {
            ScheduleChanged(hSource, m_instances.FastGet(hSource.Value), true);
          }
        }

        // LOCAL_DO_SANITY_CHECK();

        LOCAL_DO_SANITY_CHECK();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_VERBOSE("Exception occurred: '{}'", ex.what());
        // Rolling back all in-progress changes
        for (const auto hSource : inProgressSourceBindings.SourceHandles())
        {
          EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(m_instances.FastGet(hSource.Value).Handles, hTarget);
        }
        throw;
      }
    }
    return changed;
  }


  bool DataBindingService::ScheduleChanged(const DataBindingInstanceHandle hChangedInstance, Internal::ServiceBindingRecord& rChangedInstance,
                                           const bool forced)
  {
    assert(rChangedInstance.Instance.IsObservable());

    bool allowChanges = true;
    if (rChangedInstance.Instance.GetState() == DataBindingInstanceState::Alive)
    {
      // only allow changes if forced or there is no source attached
      allowChanges = forced || !rChangedInstance.HasValidSourceHandles();
      if (allowChanges && !rChangedInstance.Handles.Empty(Internal::ServicePropertyVectorIndex::Targets) &&
          !rChangedInstance.Instance.HasPendingChanges())
      {
        rChangedInstance.Instance.MarkPendingChanges();
        m_changes.push(hChangedInstance);
      }
    }
    return allowChanges;
  }


  bool DataBindingService::SanityCheck() const
  {
    for (uint32_t i = 0; i < m_instances.Count(); ++i)
    {
      const Internal::ServiceBindingRecord& record = m_instances[i];
      const auto hCurrent = DataBindingInstanceHandle(m_instances.FastIndexToHandle(i));

      // Validate that all handle properties are valid
      for (const auto handle : record.Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Properties))
      {
        if (m_instances.TryGet(handle.Value) == nullptr)
        {
          return false;
        }
      }
      for (const auto handle : record.Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
      {
        // Verify that the instance exist
        const auto* pRecord = m_instances.TryGet(handle.Value);
        if (pRecord == nullptr)
        {    // The record no longer exist so the To entries are corrupt
          return false;
        }
        // Verify that the handle exist in the found Source
        if (!pRecord->ContainsSource(hCurrent))
        {    // The handle was not found (so the two way linking is corrupt)
          return false;
        }
      }

      {
        // Verify that the instance exist
        for (auto sourceHandle : record.SourceHandles())
        {
          const Internal::ServiceBindingRecord* pSourceRecord = m_instances.TryGet(sourceHandle.Value);
          if (pSourceRecord == nullptr)
          {    // The record no longer exist so the Source entries are corrupt
            return false;
          }
          // Verify that the handle exist in the found To
          const auto sourceRecordTargetSpan = pSourceRecord->Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets);
          if (std::find(sourceRecordTargetSpan.begin(), sourceRecordTargetSpan.end(), hCurrent) == sourceRecordTargetSpan.end())
          {    // The handle was not found (so the two way linking is corrupt)
            return false;
          }
        }
      }
    }
    return true;
  }


  DataBindingInstanceHandle DataBindingService::DoCreateDependencyObjectProperty(
    const DataBindingInstanceHandle hOwner, [[maybe_unused]] const DependencyPropertyDefinition& propertyDefinition,
    std::unique_ptr<Internal::IPropertyMethods> methods, const DataBindingInstanceType propertyType, const Internal::InstanceState::Flags flags)
  {
    assert(m_callContext.State == CallContextState::Idle || m_callContext.State == CallContextState::ExecutingObserverCallbacks);
    assert(propertyType == DataBindingInstanceType::DependencyProperty || propertyType == DataBindingInstanceType::DependencyObserverProperty);

    // We can not cache the returned record as we do a add to m_instances before we need to write to it
    {
      auto ownerType = m_instances.Get(hOwner.Value).Instance.GetType();
      if (ownerType != DataBindingInstanceType::DependencyObject && ownerType != DataBindingInstanceType::DependencyProperty)
      {
        throw InvalidParentInstanceException(
          "hDependencyObject must be a DataBindingInstanceType::DependencyObject or DataBindingInstanceType::DependencyProperty");
      }
    }
    EnsureDestroyCapacity();

    const Internal::PropertyMethodsImplType methodsImplType = methods->GetImplType();
    DataBindingInstanceHandle hNew(m_instances.Add(Internal::ServiceBindingRecord(propertyType, methodsImplType, flags, std::move(methods))));
    try
    {
      Internal::ServiceBindingRecord& rParentInstance = m_instances.FastGet(hOwner.Value);
      rParentInstance.Handles.PushBack(Internal::ServicePropertyVectorIndex::Properties, hNew);
    }
    catch (const std::exception&)
    {
      DoDestroyInstanceNow(hNew);
      assert(SanityCheck());
      throw;
    }
    LOCAL_DO_SANITY_CHECK();
    return hNew;
  }

  void DataBindingService::ExecutePendingBindingsNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    LOCAL_DO_SANITY_CHECK();

    try
    {
      m_callContext.State = CallContextState::ExecutePendingBindings;
      while (!m_pendingBindings.empty())
      {
        const auto pendingBinding = m_pendingBindings.front();
        m_pendingBindings.pop();
        DoSetBinding(pendingBinding.TargetHandle, Binding(pendingBinding.SourceHandle));
        m_pendingObserverCallbacks.emplace(pendingBinding.TargetHandle, pendingBinding.SourceHandle);
      }
      m_callContext = {};
    }
    catch (const std::exception&)
    {
      m_callContext = {};
      FSLLOG3_ERROR("Exeception during DataBinding ExecuteObserverCallbacksNow");
      throw;
    }
    LOCAL_DO_SANITY_CHECK();
  }


  void DataBindingService::ExecuteObserverCallbacksNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    assert(!m_callContext.Handle.IsValid());

    if (!m_pendingObserverCallbacks.empty())
    {
      LOCAL_DO_SANITY_CHECK();

      try
      {
        m_callContext.State = CallContextState::ExecutingObserverCallbacks;
        while (!m_pendingObserverCallbacks.empty())
        {
          const auto observerRecord = m_pendingObserverCallbacks.front();
          m_pendingObserverCallbacks.pop();
          ExecuteInstanceObserverCallback(observerRecord.TargetHandle, observerRecord.SourceHandle);
        }
        m_callContext = {};
      }
      catch (const std::exception&)
      {
        m_callContext = {};
        FSLLOG3_ERROR("Exeception during DataBinding ExecuteObserverCallbacksNow");
        throw;
      }
      LOCAL_DO_SANITY_CHECK();
    }
  }

  // Beware this modifies the m_pendingObserverCallbacks with observer instances that need to be executed
  void DataBindingService::ExecutePendingChangesNow()
  {
    assert(m_callContext.State == CallContextState::Idle);
    assert(!m_callContext.Handle.IsValid());

    if (!m_changes.empty())
    {
      LOCAL_DO_SANITY_CHECK();

      try
      {
        m_callContext.State = CallContextState::ExecutingChanges;
        assert(m_pendingObserverCallbacks.empty());
        while (!m_changes.empty())
        {
          const auto hChangedInstance = m_changes.front();
          m_changes.pop();
          auto* pChangedInstance = m_instances.TryGet(hChangedInstance.Value);
          if (pChangedInstance != nullptr)
          {
            assert(pChangedInstance->Instance.HasPendingChanges());
            pChangedInstance->Instance.ClearPendingChanges();

            // this method does not modify m_instances so the reference "pChangedInstance" into it is safe
            ExecuteChangesTo(hChangedInstance, *pChangedInstance);
          }
        }
        m_callContext = {};
      }
      catch (const std::exception&)
      {
        m_callContext = {};
        FSLLOG3_ERROR("Exeception during DataBinding ExecutPendingChangesNow");
        throw;
      }
      LOCAL_DO_SANITY_CHECK();
    }
  }

  // Beware this modifies the m_pendingObserverCallbacks with observer instances that need to be executed
  void DataBindingService::ExecuteChangesTo(const DataBindingInstanceHandle hSource, const Internal::ServiceBindingRecord& source)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    if (source.Instance.GetState() == DataBindingInstanceState::Alive)
    {
      // Execute the change
      for (const auto hTarget : source.Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
      {
        const auto& target = m_instances.FastGet(hTarget.Value);
        if (target.Instance.GetState() == DataBindingInstanceState::Alive)
        {
          bool changed = false;
          switch (target.Instance.GetType())
          {
          case DataBindingInstanceType::DependencyObserverProperty:
            m_pendingObserverCallbacks.emplace(hTarget, hSource);
            break;
          // case DataBindingInstanceType::DependencyObject:
          // case DataBindingInstanceType::Target:
          //  assert(target.pDataTarget != nullptr);
          //  target.pDataTarget->OnChanged(hChangedInstance);
          //  break;
          case DataBindingInstanceType::DependencyProperty:
            changed = ExecuteDependencyPropertyGetSet(hTarget, target, source);
            break;
          default:
            throw InternalErrorException("Change to a object of a unsupported type");
          }
          if (changed)
          {
            // Execute any pending changes to dependent resources
            ExecuteChangesTo(hTarget, target);
          }
        }
        else
        {
          FSLLOG3_ERROR("Encountered dead to instance, that ought to have been filtered before");
        }
      }
    }
    else
    {
      FSLLOG3_ERROR("Encountered dead changed instance, that ought to have been filtered before");
    }
  }


  void DataBindingService::ExecuteInstanceObserverCallback(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource)
  {
    assert(m_callContext.State == CallContextState::ExecutingObserverCallbacks);
    assert(hTarget.IsValid());
    assert(hSource.IsValid());
    try
    {
      Internal::IPropertyMethods* pMethods = nullptr;
      {
        const Internal::ServiceBindingRecord& record = m_instances.FastGet(hTarget.Value);
        assert(record.Instance.GetType() == DataBindingInstanceType::DependencyObserverProperty);
        assert(record.Methods);
        if (record.Instance.GetState() == DataBindingInstanceState::Alive)
        {
          pMethods = record.Methods.get();
        }
        else
        {
          FSLLOG3_VERBOSE4("observed callback object is dead, so skipping callback");
        }
      }
      if (pMethods != nullptr)
      {
        if (!pMethods->TryInvoke(hSource))
        {
          FSLLOG3_WARNING("Invoke failed");
        }
        // The source should not have been deleted while executing this
        assert(m_instances.IsValidHandle(hSource.Value));
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Exeception occurred during ExecuteInstanceObserverCallback: {}", ex.what());
      throw;
    }
  }


  bool DataBindingService::ExecuteDependencyPropertyGetSet(const DataBindingInstanceHandle hTarget, const Internal::ServiceBindingRecord& target,
                                                           const Internal::ServiceBindingRecord& source)
  {
    assert(m_callContext.State == CallContextState::ExecutingChanges);
    assert(hTarget.IsValid());
    assert(source.Methods);
    assert(target.Methods);
    Internal::PropertySetResult setResult{Internal::PropertySetResult::NotSupported};
    try
    {
      {
        m_callContext.Handle = hTarget;
        if (!target.SourceUserBinding())
        {
          // We assume we have a both a get and set
          setResult = target.Methods->TrySet(source.Methods.get());
        }
        else
        {
          auto* const pConverter = dynamic_cast<AConverterBinding*>(target.SourceUserBinding().get());
          if (pConverter != nullptr)
          {
            setResult = pConverter->Convert(target.Instance.GetPropertyMethodsImplType(), target.Methods.get(),
                                            source.Instance.GetPropertyMethodsImplType(), source.Methods.get());
          }
          else
          {
            setResult = ExecuteDependencyPropertyGetSetMultiConverterBinding(m_instances, target);
          }
        }
        m_callContext.Handle = {};
      }
      assert(m_callContext.State == CallContextState::ExecutingChanges);
    }
    catch (const std::exception& ex)
    {
      assert(m_callContext.State == CallContextState::ExecutingChanges);
      m_callContext.Handle = {};
      FSLLOG3_ERROR("Exeception occurred during ExecuteDependencyPropertyGetSet callback: {}", ex.what());
      throw;
    }

    switch (setResult)
    {
    case Internal::PropertySetResult::ValueUnchanged:
      return false;
    case Internal::PropertySetResult::ValueChanged:
      return true;
    case Internal::PropertySetResult::UnsupportedGetType:
      throw NotSupportedException("Unsupported get type (this should not occur)");
    case Internal::PropertySetResult::UnsupportedSetType:
      throw NotSupportedException("Unsupported set type (this should not occur)");
    case Internal::PropertySetResult::UnsupportedBindingType:
      throw NotSupportedException("Unsupported binding type (this should not occur)");
    case Internal::PropertySetResult::NotSupported:
      throw NotSupportedException("Set is unsupported (this should not occur)");
    }
    throw InternalErrorException(fmt::format("Unsupported setResult: {}", static_cast<int32_t>(setResult)));
  }


  void DataBindingService::DestroyScheduledNow() noexcept
  {
    for (const auto hDestroy : m_scheduledForDestroy)
    {
      DoDestroyInstanceNow(hDestroy);
    }
    m_scheduledForDestroy.clear();
  }


  void DataBindingService::EnsureDestroyCapacity()
  {
    std::size_t desiredCapacity(m_instances.Count());
    ++desiredCapacity;

    if (desiredCapacity > m_scheduledForDestroy.capacity())
    {
      m_scheduledForDestroy.reserve(desiredCapacity + 128);
    }
  }


  void DataBindingService::DoDestroyInstanceNow(DataBindingInstanceHandle hInstance) noexcept
  {
    auto* pRecord = m_instances.TryGet(hInstance.Value);
    if (pRecord != nullptr)
    {
      pRecord->Instance.SetDataBindingInstanceState(DataBindingInstanceState::Destroyed);
      FSLLOG3_VERBOSE4("Destroying instance {}", hInstance.Value);

      // Remove all bindings that use this instance as a target
      if (pRecord->HasValidSourceHandles())
      {
        // This will not add or remove elements from m_instances so our pRecord pointer is still valid after the call
        ClearSourceBindings(m_instances, hInstance);
      }

      // Remove all bindings that use this instance as a source
      for (const auto hTarget : pRecord->Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
      {
        Internal::ServiceBindingRecord& rToRecord = m_instances.FastGet(hTarget.Value);
        assert(rToRecord.ContainsSource(hInstance));
        // This warning could safely occur during 'shutdown' so we need a way to eliminate false warnings.
        if (rToRecord.SourceHandleCount() > 1u)
        {
          // FSLLOG3_DEBUG_WARNING("- Clearing multibind source because one of the linked sources was destroyed");
          for (const DataBindingInstanceHandle srcHandle : rToRecord.SourceHandles())
          {
            FSLLOG3_VERBOSE4("- Remove source {} from {}", srcHandle.Value, hTarget.Value);
            Internal::ServiceBindingRecord& rMultiBindSource = m_instances.FastGet(srcHandle.Value);
            EraseFirst<Internal::ServicePropertyVectorIndex::Targets>(rMultiBindSource.Handles, hTarget);
          }
        }
        else
        {
          FSLLOG3_VERBOSE4("- Remove source {} from {}", hInstance.Value, hTarget.Value);
        }
        rToRecord.ClearSourceHandles();
      }
      pRecord->Handles.Clear(Internal::ServicePropertyVectorIndex::Targets);

      // Destroy all properties of this instance
      for (const auto hProperty : pRecord->Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Properties))
      {
        DoDestroyInstanceNow(hProperty);
      }
      pRecord->Handles.Clear(Internal::ServicePropertyVectorIndex::Properties);

      m_instances.FastRemoveAt(m_instances.FastHandleToIndex(hInstance.Value));
    }
  }


  void DataBindingService::CheckForCyclicDependencies(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource) const
  {
    if (hTarget == hSource)
    {
      throw CyclicBindingException("Circular dependency found (cant bind to itself)");
    }

    const auto& targetRecord = m_instances.FastGet(hTarget.Value);
    // The target is not expected to have any existing source
    assert(!targetRecord.HasValidSourceHandles());

    for (const auto entry : targetRecord.Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
    {
      if (IsInstanceTarget(entry, hSource))
      {
        throw CyclicBindingException("Circular dependency found");
      }
    }
  }


  bool DataBindingService::IsInstanceTarget(const DataBindingInstanceHandle hInstance, const DataBindingInstanceHandle hEntry) const
  {
    bool isTarget = hInstance == hEntry;
    if (!isTarget)
    {
      const auto& record = m_instances.FastGet(hInstance.Value);
      if (record.Instance.GetState() == DataBindingInstanceState::Alive)
      {
        for (const auto entry : record.Handles.AsReadOnlySpan(Internal::ServicePropertyVectorIndex::Targets))
        {
          if (IsInstanceTarget(entry, hEntry))
          {
            isTarget = true;
            break;
          }
        }
      }
    }
    return isTarget;
  }
}
