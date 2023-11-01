// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "ItemDataHandler.generated.h"

class IStompMessage;

/**
 *  물품에 대한 데이터
 */
USTRUCT()
struct FItemData
{
	GENERATED_BODY()

	FItemData() : ItemID(0), SellerName(TEXT("")), BuyerName(TEXT("")), Title(TEXT("")), Information(TEXT("")),
	Location(0), World(TEXT("")), Type(EItemDealType::None), ImgCount(0), StartPrice(0), CurrentPrice(0), HaveGlb(false) {}

	// 물품 ID
	UPROPERTY()
	uint32 ItemID;
	
	// 판매자 이름
	UPROPERTY()
	FString SellerName;

	// 구매자 이름
	UPROPERTY()
	FString BuyerName;
	
	// 물품 제목
	UPROPERTY()
	FString Title;

	// 물품 상세 정보
	UPROPERTY()
	FString Information;

	// 위치
	UPROPERTY()
	uint8 Location;

	// 어디서 판매중인지 web : 웹에서만, 메타버스에서도 판매중이면 월드 번호
	UPROPERTY()
	FString World;

	// 판매 타입이 무엇인지, auction or normal
	UPROPERTY()
	EItemDealType Type;
	
	// 이미지 파일의 갯수
	UPROPERTY()
	uint8 ImgCount;
	
	// 물품 경매 시작 가격
	UPROPERTY()
	uint64 StartPrice;

	// 물품 경매 현재 가격
	UPROPERTY()
	uint64 CurrentPrice;

	// 물품 경매 현재 가격
	UPROPERTY()
	uint8 HaveGlb:1;
	
	// 종료 시각. 년 / 월 / 일 / 시간 / 분 
	UPROPERTY()
	FDateTime EndTime;

	// 최종 정보가 수정된 시간. 년 / 월 / 일 / 시간 / 분 / 초
	UPROPERTY()
	FDateTime LastModifyTime;
};


/**
 *  물품 검색 시 사용할 Option struct
 *  필요한 옵션만 넣어서 사용하면 됩니다.
 */
USTRUCT()
struct FItemSearchOption
{
	GENERATED_BODY()

	FItemSearchOption() : SearchString(TEXT("")), World(TEXT("")), ItemType(EItemDealType::None), CanDeal(EItemCanDeal::None) {}
	
	// 제목 또는 내용에 해당 문자열이 포함되어 있는지 확인
	FString SearchString;

	// 어떤 월드에 해당하는 물품인지
	FString World;

	// 판매 타입이 어떤것인지
	EItemDealType ItemType;

	// 현재 판매중인(구매가 가능한) 상품인지
	EItemCanDeal CanDeal;
};

/**
 *  어떠한 물품 ID의 입찰 기록에 대한 정보를 담는 구조체
 */
USTRUCT()
struct FBidRecord
{
	GENERATED_BODY()
	
	// 입찰 순서
	uint32 Order;

	// 입찰 가격
	uint64 BidPrice;

	// 입찰 시각
	FDateTime BidTime;

	// 입찰한 사람
	FString BidUser;
};

/**
 * 물품의 data를 다루는 Handler class입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UItemDataHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UItemDataHandler();

protected:
	virtual void BeginPlay() override;
	
public:
	// ItemID로 물품 정보를 요청합니다. 
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestItemDataById(FCallbackOneParam<const FItemData&> InFunc, uint32 InItemId) const;

	// 옵션을 걸어 물품 정보들을 요청합니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestItemDataByOption(FCallbackRefArray<FItemData> InFunc, const FItemSearchOption& InSearchOption) const;

	// HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
	void Client_RequestBid(uint32 InItemId, uint64 InPrice) const;

	// ItemID로 물품에 대한 입찰 기록을 조회합니다. DB에서 입찰 순서대로 내림차순(최근 입찰 기록이 먼저)해서 정렬하여 주니, 그대로 배열을 사용하시면 됩니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestBidRecordByItemId(FCallbackRefArray<FBidRecord> InFunc, uint32 InItemId) const;

	// ItemId로 물품을 삭제합니다. (로그인 된 상태여야 함), (JWT 토큰 확인으로 내 물품이 맞을 경우만 삭제함, 판매 종료 3시간 전에는 삭제 불가)
	void RequestRemoveItem(uint32 InItemId) const;

	// Type : Sell (내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청합니다. 최근에 업로드한 물품이 배열의 앞) 
	// Type : Buy (내가 구매 성공한 물품을 요청합니다. 최근에 업로드한 물품이 배열의 앞)
	// Type : TryBid ( 현재 판매중인 물품이면서, 내가 입찰을 시도 했던 물품. 최근 입찰한 것이 배열 앞)
	// (로그인 된 상태여야함)
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestMyItem(FCallbackRefArray<FItemData> InFunc, EMyItemReqType InMyItemReqType) const;

	// Stomp로 item id에 맞는 상품에 댓글을 답니다.
	// 로그인 된 경우만 사용 가능
	void AddReply(const uint32 InItemId, const FString& InContent) const;
private:
	// Stomp 메세지로 온 아이템 가격 변동 알림을 받는다.
	void _Server_OnChangePrice(const IStompMessage& InMessage) const;

	// Stomp 메세지로 아이템 정보 변동 알림을 받는다.
	void _Server_OnChangeItemData(const IStompMessage& InMessage) const;
	
	// 서버 -> RPC로 모든 클라에게 가격 변동 알림을 줍니다.
	UFUNCTION( NetMulticast, Reliable )
	void _MulticastRPC_ChangePrice(const uint32& InItemId, const uint64& InPrice) const;

	// 서버 -> RPC로 모든 클라에게 아이템 정보 변동 알림을 줍니다.
	UFUNCTION( NetMulticast, Unreliable )
	void _MulticastRPC_ChangeItemData(const uint32& InItemId, const FString& InWorld, const FString& InChangeList) const;
	
	// Json 형태의 ItemData JsonObject를 FItemData로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const;

	// Json 형태의 BidRecord JsonObject를 FBidRecord로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FBidRecord& OutBidRecord) const;

public:
	// 가격 변동 알림을 받아내는 Delegate
	// UI서 아이템 확인시 구독, 확인 끝날시 구독 해제를 통해서 실시간으로 가격이 변동되게 해주세요.
	// 물품 id , 변동된 이후 가격이 broadcast 됩니다.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangePrice, const uint32&, const uint64&)
	FOnChangePrice OnChangePrice;

	// 아이템 정보 변동 알림을 받아내는 Delegate
	// UI서 아이템 확인시 구독, 확인 끝날시 구독 해제를 통해서 실시간으로 정보가 변동되게 해주세요(glb제외).
	// 변경된 item ID, 그 물품이 있던 world, 바뀐 항목에 대한 string ( -로 구분, (ex)location-world )이 broadcast 됩니다.
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeItemData, const uint32&, const FString&, const FString&)
	FOnChangeItemData OnChangeItemData;
};
