#ifndef __DACE_VIEW_H
#define __DACE_VIEW_H

#include <cstdint>

#include "types.h"
#include "vector.h"
#include "reduction.h"

namespace dace {

    template <typename T, uint8_t DIMS, int VECTOR_LEN = 1,
        int NUM_ACCESSES = static_cast<int>(NA_RUNTIME), bool ALIGNED = false,
        typename OffsetT = int32_t>
    class ArrayViewIn
    {
    protected:

     template <int VECTOR_LEN_OTHER>
     using vec_other_t = typename std::conditional<ALIGNED, vec<T, VECTOR_LEN>,
                                                   vecu<T, VECTOR_LEN>>::type;

     template <int VECTOR_LEN_OTHER>
     using vecu_other_t = typename std::conditional<false, vec<T, VECTOR_LEN>,
                                                    vecu<T, VECTOR_LEN>>::type;

     // The internal pointer type relies on the alignment of the original array
     using vec_t = typename std::conditional<ALIGNED, vec<T, VECTOR_LEN>,
                                             vecu<T, VECTOR_LEN>>::type;

     T const* m_ptr;
     OffsetT m_stride[DIMS];

    public:
        template <typename... Dim>
        explicit DACE_HDFI ArrayViewIn(T const* ptr, const Dim&... strides) : 
            m_ptr(ptr) {
            static_assert(sizeof...(strides) == static_cast<int>(DIMS),
                          "Dimension mismatch");
            OffsetT stridearr[] = { static_cast<OffsetT>(strides)... };
            for (int i = 0; i < DIMS; ++i)
                m_stride[i] = stridearr[i];
        }

        template <typename... Dim>
        DACE_HDFI const vec_t& operator()(const Dim&... indices) const {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return get_element(index_array);
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> const* ptr(
            T const* _ptr) const {
            return reinterpret_cast<vecu_other_t<VECTOR_LEN_OTHER> const*>(
                _ptr);
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> const* ptr() const {
            return ptr<VECTOR_LEN_OTHER>(m_ptr);
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vec_other_t<VECTOR_LEN_OTHER> const& ref() const {
            return *ptr<VECTOR_LEN_OTHER>();
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vec_other_t<VECTOR_LEN_OTHER> val() const {
            return *ptr<VECTOR_LEN_OTHER>();
        }

        // template <typename... Dim>
        // DACE_HDFI T const* ptr_at(const Dim&... indices) const {
        //     static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
        //     OffsetT index_array[] = { static_cast<OffsetT>(indices)... };
        //     OffsetT offset;
        //     get_offset(index_array, offset);
        //     return m_ptr + offset;
        // }

    protected:
        DACE_HDFI void get_offset(OffsetT(&index_array)[DIMS],
                                  OffsetT& offset) const {
            offset = 0;
            for (int i = 0; i < DIMS - 1; ++i) {
                offset += index_array[i] * m_stride[i];
            }
            offset += index_array[DIMS - 1] * m_stride[DIMS - 1] * VECTOR_LEN;
        }

        DACE_HDFI const vec_t& get_element(OffsetT(&index_array)[DIMS]) const {
            OffsetT offset;
            get_offset(index_array, offset);
            return *ptr(m_ptr + offset);
        }
    };
    
    template <typename T, uint8_t DIMS, int VECTOR_LEN = 1,
        int NUM_ACCESSES = static_cast<int>(NA_RUNTIME), bool ALIGNED = false,
        typename OffsetT = int32_t>
    class ArrayViewOut
    {
    protected:
        // The internal pointer type relies on the alignment of the original array
        using vec_t = typename std::conditional<ALIGNED, vec<T, VECTOR_LEN>,
            vecu<T, VECTOR_LEN>>::type;

        template <int VECTOR_LEN_OTHER>
        using vecu_other_t = vecu<T, VECTOR_LEN_OTHER>;

        T* m_ptr;
        OffsetT m_stride[DIMS];

    public:
        template <typename... Dim>
        explicit DACE_HDFI ArrayViewOut(T* ptr, const Dim&... strides) : 
            m_ptr(ptr) {
            static_assert(sizeof...(strides) == static_cast<int>(DIMS),
                          "Dimension mismatch");
            OffsetT stridearr[] = { static_cast<OffsetT>(strides)... };
            for (int i = 0; i < DIMS; ++i)
                m_stride[i] = stridearr[i];
        }

        template <typename... Dim>
        DACE_HDFI const vec_t& operator()(const Dim&... indices) const {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return get_element(index_array);
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> const* ptr(
            T const* _ptr) const {
          if (VECTOR_LEN_OTHER == VECTOR_LEN) {
            return reinterpret_cast<vecu_other_t<VECTOR_LEN_OTHER> const*>(
                _ptr);
          }
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER>* ptr(T *_ptr) const {
          if (VECTOR_LEN_OTHER == VECTOR_LEN) {
            return reinterpret_cast<vecu_other_t<VECTOR_LEN_OTHER>*>(_ptr);
          }
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> const* ptr() const {
          if (VECTOR_LEN_OTHER == VECTOR_LEN) {
              return ptr<VECTOR_LEN_OTHER>(m_ptr);
          }
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER>* ptr() {
          if (VECTOR_LEN_OTHER == VECTOR_LEN) {
            return ptr<VECTOR_LEN_OTHER>(m_ptr);
          }
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> const& ref() const {
            return *ptr<VECTOR_LEN_OTHER>();
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> & ref() {
            return *ptr<VECTOR_LEN_OTHER>();
        }

        template <int VECTOR_LEN_OTHER = VECTOR_LEN>
        DACE_HDFI vecu_other_t<VECTOR_LEN_OTHER> val() const {
            return *ptr<VECTOR_LEN_OTHER>();
        }

        template <typename... Dim>
        DACE_HDFI void write(const vec_t& value, const Dim&... indices) {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            set_element(value, index_array);
        }

        template <typename CONFLICT_RESOLUTION, typename... Dim>
        DACE_HDFI vec_t write_and_resolve(const vec_t& value, CONFLICT_RESOLUTION wcr,
                                         const Dim&... indices) {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return set_element_wcr(value, index_array, wcr);
        }

        template <typename CONFLICT_RESOLUTION, typename... Dim>
        DACE_HDFI vec_t write_and_resolve_nc(const vec_t& value,
                                            CONFLICT_RESOLUTION wcr,
                                            const Dim&... indices) {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return set_element_wcr_nc(value, index_array, wcr);
        }

        template <ReductionType REDT, typename... Dim>
        DACE_HDFI vec_t write_and_resolve(const vec_t& value,
                                         const Dim&... indices) {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return set_element_wcr<REDT>(value, index_array);
        }

        template <ReductionType REDT, typename... Dim>
        DACE_HDFI vec_t write_and_resolve_nc(const vec_t& value,
                                            const Dim&... indices) {
            static_assert(sizeof...(indices) == DIMS, "Dimension mismatch");
            OffsetT index_array[] = { static_cast<OffsetT>(indices)... };

            return set_element_wcr_nc<REDT>(value, index_array);
        }

    protected:
        DACE_HDFI void get_offset(OffsetT(&index_array)[DIMS],
                                  OffsetT& offset) const {
            offset = 0;
            for (int i = 0; i < DIMS - 1; ++i) {
                offset += index_array[i] * m_stride[i];
            }
            offset += index_array[DIMS - 1] * m_stride[DIMS - 1] * VECTOR_LEN;
        }

        DACE_HDFI const vec_t& get_element(OffsetT(&index_array)[DIMS]) const {
            OffsetT offset;
            get_offset(index_array, offset);
            return *ptr(m_ptr + offset);
        }

        DACE_HDFI void set_element(const vec_t& value, OffsetT(&index_array)[DIMS]) {
            OffsetT offset;
            get_offset(index_array, offset);
            *ptr(m_ptr + offset) = value;
        }

        template <ReductionType REDT>
        DACE_HDFI vec_t set_element_wcr(const vec_t& value,
                                       OffsetT(&index_array)[DIMS]) {
            OffsetT offset;
            get_offset(index_array, offset);

            return wcr_fixed<REDT, vec_t>::reduce_atomic(ptr<VECTOR_LEN>(
                m_ptr + offset), value);
        }

        template <ReductionType REDT>
        DACE_HDFI vec_t set_element_wcr_nc(const vec_t& value,
                                          OffsetT(&index_array)[DIMS]) {
            OffsetT offset;
            get_offset(index_array, offset);

            return wcr_fixed<REDT, vec_t>::reduce(ptr(m_ptr + offset), value);
        }

        template <typename WCR_T>
        DACE_HDFI vec_t set_element_wcr(const vec_t& value,
                                       OffsetT(&index_array)[DIMS], WCR_T wcr) {
            OffsetT offset;
            get_offset(index_array, offset);

            return wcr_custom<vec_t>::template reduce_atomic(
                wcr, ptr(m_ptr + offset), value);
        }

        template <typename WCR_T>
        DACE_HDFI vec_t set_element_wcr_nc(const vec_t& value,
                                          OffsetT(&index_array)[DIMS], WCR_T wcr) {
            OffsetT offset;
            get_offset(index_array, offset);

            return wcr_custom<vec_t>::template reduce(
                wcr, ptr(m_ptr + offset), value);
        }
    };

    // Scalar version
    template <typename T, int VECTOR_LEN, int NUM_ACCESSES, bool ALIGNED,
        typename OffsetT>
        class ArrayViewIn<T, 0, VECTOR_LEN, NUM_ACCESSES, ALIGNED, OffsetT> {
        protected:
            // The internal pointer type relies on the alignment of the original array
            // using vec_t = vec<T, VECTOR_LEN>;
            using vec_t = vecu<T, VECTOR_LEN>;

            T const *m_ptr;

        public:

            explicit DACE_HDFI ArrayViewIn(T const *ptr) : m_ptr(ptr) {}

            // Template on int to conform to the same interface as non-scalar,
            // but only accept the native vector length
            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const* ptr(T const *_ptr) const {
                return reinterpret_cast<vec_t const*>(_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const* ptr() const {
                return ptr<VECTOR_LEN_OTHER>(m_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const& ref() const {
                return *ptr<VECTOR_LEN_OTHER>();
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t val() const {
                return *ptr<VECTOR_LEN_OTHER>();
            }

            DACE_HDFI operator vec_t() const {
                return val<VECTOR_LEN>();
            }
    };

    // Scalar version
    template <typename T, int VECTOR_LEN, int NUM_ACCESSES, bool ALIGNED,
        typename OffsetT>
        class ArrayViewOut<T, 0, VECTOR_LEN, NUM_ACCESSES, ALIGNED, OffsetT> {
        protected:
            // The internal pointer type relies on the alignment of the original array
            // using vec_t = vec<T, VECTOR_LEN>;
            using vec_t = vecu<T, VECTOR_LEN>;

            T* m_ptr;

        public:
            explicit DACE_HDFI ArrayViewOut(T* ptr) : m_ptr(ptr) {}

            // Template on int to conform to the same interface as non-scalar,
            // but only accept the native vector length
            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const* ptr(T const *_ptr) const {
                return reinterpret_cast<vec_t const*>(_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t *ptr(T *_ptr) const {
                return reinterpret_cast<vec_t *>(_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const* ptr() const {
                return ptr<VECTOR_LEN_OTHER>(m_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t *ptr() {
                return ptr<VECTOR_LEN_OTHER>(m_ptr);
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t const& ref() const {
                return *ptr<VECTOR_LEN_OTHER>();
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t &ref() {
                return *ptr<VECTOR_LEN_OTHER>();
            }

            template <int VECTOR_LEN_OTHER = VECTOR_LEN>
            DACE_HDFI vec_t val() const {
                return *ptr<VECTOR_LEN_OTHER>();
            }

            DACE_HDFI operator vec_t() const {
                return val<VECTOR_LEN>();
            }

            DACE_HDFI void write(const vec_t& value) {
                *ptr<VECTOR_LEN>() = value;
            }

            template <typename CONFLICT_RESOLUTION>
            DACE_HDFI vec_t write_and_resolve(const vec_t& value,
                                             CONFLICT_RESOLUTION wcr) {
                return wcr_custom<vec_t>::reduce_atomic(
                    wcr, ptr<VECTOR_LEN>(), value);
            }

            template <typename CONFLICT_RESOLUTION>
            DACE_HDFI vec_t write_and_resolve_nc(const vec_t& value,
                                             CONFLICT_RESOLUTION wcr) {
                return wcr_custom<vec_t>::template reduce(
                    wcr, ptr<VECTOR_LEN>(), value);
            }

            template <ReductionType REDT>
            DACE_HDFI vec_t write_and_resolve(const vec_t& value) {
                return wcr_fixed<REDT, vec_t>::reduce_atomic(
                    ptr<VECTOR_LEN>(), value);
            }

            template <ReductionType REDT>
            DACE_HDFI vec_t write_and_resolve_nc(const vec_t& value) {
                return wcr_fixed<REDT, vec_t>::reduce(ptr<VECTOR_LEN>(), value);
            }

            // Special case for vector conditionals
#define VECTOR_CONDITIONAL_WRITE_AND_RESOLVE(N)                             \
            template <typename CONFLICT_RESOLUTION>                         \
            DACE_HDFI vec<int, N> write_and_resolve(const vec<int, N>& value,      \
                                             CONFLICT_RESOLUTION wcr) {     \
                int ppcnt = 0;                                              \
                for (int v = 0; v < N; ++v) ppcnt += value[v] ? 1 : 0;      \
                return write_and_resolve(ppcnt)                             \
            }                                                               \
            template <ReductionType REDT>                                 \
            DACE_HDFI vec<int, N> write_and_resolve(const vec<int, N>& value) {    \
                int ppcnt = 0;                                              \
                for (int v = 0; v < N; ++v) ppcnt += value[v] ? 1 : 0;      \
                return write_and_resolve<REDT>(ppcnt)                       \
            }                                                               \
            template <typename CONFLICT_RESOLUTION>                         \
            DACE_HDFI vec<int, N> write_and_resolve_nc(const vec<int, N>& value,   \
                                                CONFLICT_RESOLUTION wcr) {  \
                int ppcnt = 0;                                              \
                for (int v = 0; v < N; ++v) ppcnt += value[v] ? 1 : 0;      \
                return write_and_resolve_nc(ppcnt)                          \
            }                                                               \
            template <ReductionType REDT>                                 \
            DACE_HDFI vec<int, N> write_and_resolve_nc(const vec<int, N>& value) { \
                int ppcnt = 0;                                              \
                for (int v = 0; v < N; ++v) ppcnt += value[v] ? 1 : 0;      \
                return write_and_resolve_nc<REDT>(ppcnt)                    \
            }

            //VECTOR_CONDITIONAL_WRITE_AND_RESOLVE(2)
            //VECTOR_CONDITIONAL_WRITE_AND_RESOLVE(4)
            //VECTOR_CONDITIONAL_WRITE_AND_RESOLVE(8)
    };

}  // namespace dace

#endif  // __DACE_VIEW_H
