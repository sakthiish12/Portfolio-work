#pragma once
#ifndef	IASSET_H
#define IASSET_H
#include <Precompiled/pch.h>

namespace BE
{
	class IAsset
	{
	public:
		IAsset() = default;
		IAsset(IAsset&&) = default;
		IAsset(const IAsset&) = default;
		IAsset& operator=(IAsset&&) = default;
		IAsset& operator=(const IAsset&) = default;
		virtual ~IAsset() = default;
	public:
		RTTR_ENABLE()
	};
}

#endif