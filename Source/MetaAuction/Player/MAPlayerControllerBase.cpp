// Fill out your copyright notice in the Description page of Project Settings.


#include "MAPlayerControllerBase.h"

/**
 *  Client를 레벨 이동 시킵니다. 경매장으로 이동 시 로그인이 되어있지 않으면 이동시키지 않습니다.
 *  @param InType : 이동할 레벨의 타입
 */
void AMAPlayerControllerBase::ClientLevelTravel(const ELevelType InType)
{
	FString Url = TEXT("");

	if(InType == ELevelType::Lobby)
		Url = DA_NETWORK(LobbyUrl);
	else if(InType == ELevelType::Auction && (MAGetMyJwtToken(GetGameInstance()) != TEXT("")))
		Url = DA_NETWORK(AuctionUrl);
	
	if(Url == TEXT(""))
		return;
	
	ClientTravel(Url, TRAVEL_Absolute, false);
}