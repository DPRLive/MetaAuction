// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h> 
#include <IStompClient.h>
#include "ItemManager.generated.h"

class AItemActor;
class FJsonObject;

/**
 *  물품 ID에 대한 데이터
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
 *  전체적인 물품들의 상태를 관리해주는 Item Manager
 *  GameState에서 관리하여 물품에 대한 관리는 server가 주도함.
 *  TODO: 물품 관리에 대한 쿼리용 함수 작성
 *  TODO : 아이템 변경 관련한 코드 작성 (지금은 websocket->dedi->client에서 broadcast 생각중
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UItemManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UItemManager();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// 상품이 삭제 되었을 때, 내 월드에 있었던 상품이면 지웁니다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_UnregisterItem(uint32 InItemId);
	
	// 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterNewItem(uint32 InItemId);
	
	// 웹서버에 등록된 현재 월드에 배치되어 판매되고 있는 아이템들의 ID를 가져와 배치한다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterAllWorldItemID();

	// ItemID로 물품 정보를 요청합니다. 
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestItemDataByID(FCallbackOneParam<const FItemData&> InFunc, uint32 InItemId);

	// 옵션을 걸어 물품 정보들을 요청합니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestItemDataByOption(FCallbackRefArray<FItemData> InFunc, const FItemSearchOption& InSearchOption);

	// HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
	void Client_RequestBid(uint32 InItemId, uint64 InPrice);

	// ItemID로 물품에 대한 입찰 기록을 조회합니다. DB에서 입찰 순서대로 내림차순(최근 입찰 기록이 먼저)해서 정렬하여 주니, 그대로 배열을 사용하시면 됩니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestBidRecordByItemId(FCallbackRefArray<FBidRecord> InFunc, uint32 InItemId);

	// ItemId로 물품을 삭제합니다. (로그인 된 상태여야 함), (JWT 토큰 확인으로 내 물품이 맞을 경우만 삭제함, 판매 종료 3시간 전에는 삭제 불가)
	void RequestRemoveItem(uint32 InItemId);

	// Type : Sell (내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청합니다.)
	// Type : Buy (내가 구매 성공한 물품을 요청합니다.)
	// id 순으로 내림차순 정렬이 되어, 최근에 업로드한 물품이 배열의 앞쪽에 등장합니다. (로그인 된 상태여야함)
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestMyItem(FCallbackRefArray<FItemData> InFunc, EMyItemReqType InMyItemReqType);

	// 현재 판매중인 물품이면서, 내가 입찰을 시도 했던 물품들을 보여줍니다. (로그인 된 상태여야함)
	// 최근에 입찰한게 배열의 앞쪽에 등장합니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void RequestMyBidItem(FCallbackRefArray<FItemData> InFunc);

private:
	// Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다.
	// 데디 서버에서만 실행 가능합니다.
	void _Server_RegisterItemByLoc(uint32 InItemId, uint8 InItemLoc);
	
	// Json 형태의 ItemData JsonObject를 FItemData로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const;

	// Json 형태의 BidRecord JsonObject를 FBidRecord로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FBidRecord& OutBidRecord) const;

	// Stomp를 통해 들어오는 새 아이템 등록 알림을 처리한다.
	void _Server_OnNewItem(const IStompMessage& InMessage);

	// Stomp를 통해 들어오는 아이템 제거 알림을 처리한다.
	void _Server_OnRemoveItem(const IStompMessage& InMessage);

	// Stomp 메세지로 온 아이템 가격 변동 알림을 받는다.
	void _Server_OnChangePrice(const IStompMessage& InMessage) const;

	// 서버 -> RPC로 모든 클라에게 가격 변동 알림을 줍니다.
	UFUNCTION( NetMulticast, Reliable )
	void _MulticastRPC_ChangePrice(const uint32& InItemId, const uint64& InPrice) const;

	// 서버 -> RPC로 모든 클라에게 아이템 정보 변동 알림을 줍니다.
	UFUNCTION( NetMulticast, Unreliable )
	void _MulticastRPC_ChangeItemData(const uint32& InItemId) const;
	
	// 현재 월드에 있던 glb가 변경된 경우, 서버 -> 모든 클라에게 그 액터 다시 그리라고 명령합니다.
	UFUNCTION( NetMulticast, Unreliable )
	void _MulticastRPC_RedrawItem(const uint8& InActorIdx);
	
	// Stomp 메세지로 아이템 정보 변동 알림을 받는다.
	void _Server_OnChangeItemData(const IStompMessage& InMessage);
public:
	// 가격 변동 알림을 받아내는 Delegate
	// UI서 아이템 확인시 구독, 확인 끝날시 구독 해제를 통해서 실시간으로 가격이 변동되게 해주세요.
	// 물품 id , 변동된 이후 가격이 broadcast 됩니다.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangePrice, const uint32&, const uint64&)
	FOnChangePrice OnChangePrice;

	// 아이템 정보 변동 알림을 받아내는 Delegate
	// UI서 아이템 확인시 구독, 확인 끝날시 구독 해제를 통해서 실시간으로 정보가 변동되게 해주세요(glb제외).
	// 변경된 item ID가 broadcast 됩니다.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeItemData, const uint32&)
	FOnChangeItemData OnChangeItemData;
	
private:
	// 물품을 배치할 수 있는 ItemActor에 대한 포인터들. 사용자에게 표시는 1부터 할거지만 접근은 인덱스 0부터 해주세요.
	// 클라이언트들에서도 읽을 수 있습니다.
	// TODO : 근데 클라이언트에서 접근할 일이 있을까?
	UPROPERTY( Replicated )
	TArray<TWeakObjectPtr<AItemActor>> ItemActors;
};