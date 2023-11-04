#pragma once

// Typedef 모음
// 추후 자료형 변경시 같은 타입들을 일괄 적용을 시키기 쉽도록 하기 위해 사용
// UPROPERTY에는 using이나 typedef를 사용할 수 없네요

// 한개의 인자를 받는 람다 타입
template<typename T>
using FCallbackOneParam = TFunction<void(T)>;

template<typename T>
using FCallbackRefOneParam = TFunction<void(const T&)>;

// const TArray<T>& 타입의 람다 타입
template<typename T>
using FCallbackRefArray = TFunction<void(const TArray<T>&)>;