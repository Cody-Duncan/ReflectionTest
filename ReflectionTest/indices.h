#pragma once


template <unsigned int... Is>
struct indices {};

template <unsigned int N, unsigned int... Is>
struct build_indices : build_indices<N - 1, N - 1, Is...> {};

template <unsigned int... Is>
struct build_indices<0, Is...> : indices<Is...>{};

