#pragma once

// Data Asset를 위한 함수 모음

#include "NetworkDataAsset.h"

namespace MADataAssetHelper
{
	/**
	* 이름으로 Data Asset을 Load하는 함수
	* Game/MetaAuction/Data/DataAsset/ 폴더에 있어야함
	* @param InAssetName : 해당 Data Asset의 이름
	*/
	template<typename T>
	const TObjectPtr<T> GetDataAsset(const FString& InAssetName)
	{
		const FString path = FString::Printf(TEXT("/Script/Engine.DataAsset'/Game/MetaAuction/Data/DataAsset/%s.%s'"), *InAssetName, *InAssetName); 
		return LoadObject<T>(nullptr, *path);
	}
}

// Network Data Asset 내부의 data를 편하게 가져오기 위한 매크로
#define DA_NETWORK( Value ) \
	MADataAssetHelper::GetDataAsset<UNetworkDataAsset>(TEXT("DA_Network"))->##Value