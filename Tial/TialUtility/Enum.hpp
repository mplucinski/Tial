#pragma once

#define TIAL_UTILITY_DECLARE_ENUM_FLAG(T)                                                                           \
	inline constexpr bool operator!(const T a) { return !static_cast<int>(a); }                                                  \
	inline constexpr T operator~(const T a) { return static_cast<T>(~static_cast<int>(a)); }                                     \
	inline constexpr T operator|(const T a, const T b) { return static_cast<T>(static_cast<int>(a) | static_cast<int>(b)); }     \
	inline constexpr T operator&(const T a, const T b) { return static_cast<T>(static_cast<int>(a) & static_cast<int>(b)); }     \
	inline constexpr T operator^(const T a, const T b) { return static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b)); }     \
	inline T& operator|=(T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<int&>(a) |= static_cast<int>(b)); }     \
	inline T& operator&=(T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<int&>(a) &= static_cast<int>(b)); }     \
	inline T& operator^=(T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<int&>(a) ^= static_cast<int>(b)); }
