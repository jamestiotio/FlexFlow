#ifndef _FLEXFLOW_KERNELS_DATATYPE_DISPATCH_H
#define _FLEXFLOW_KERNELS_DATATYPE_DISPATCH_H

#include "accessor.h"

namespace FlexFlow {

template <template <DataType> class F, 
          typename ...Args,
          typename Out = decltype(std::declval<F<DT_FLOAT>>()(std::declval<Args>()...))
         >
Out dispatch(DataType dt, Args&&... args) {
  switch (dt) {
    case DT_FLOAT:
      return F<DT_FLOAT>{}(std::forward<Args>(args)...);
    case DT_DOUBLE:
      return F<DT_DOUBLE>{}(std::forward<Args>(args)...);
    case DT_INT32:
      return F<DT_INT32>{}(std::forward<Args>(args)...);
    case DT_INT64:
      return F<DT_INT64>{}(std::forward<Args>(args)...);
    case DT_BOOLEAN:
      return F<DT_BOOLEAN>{}(std::forward<Args>(args)...);
    default:
      throw mk_runtime_error("Unknown datatype {}", dt);
  }
}

template <template <DataType> class F>
struct DataTypeDispatch1 {
  template <DataType DT>
  struct Type1Dispatch {
    template <typename ...Args, typename Out = decltype(std::declval<F<DT_FLOAT>>()(std::declval<Args>()...))>
    Out operator()(Args... args) const { 
      return F<DT>{}(std::forward<Args>(args)...);
    }
  };
  
  template <typename ...Args, typename Out = decltype(std::declval<F<DT_FLOAT>>()(std::declval<Args>()...))>
  Out operator()(DataType data_type, Args... args) {
    return dispatch<Type1Dispatch>(data_type, std::forward<Args>(args)...);
  }
};

template <template <DataType, DataType> class F>
struct DataTypeDispatch2 {
  template <DataType IT>
  struct InputType {

    template <DataType OT>
    struct OutputType {
      template <typename ...Args>
      void operator()(Args ...args) const {
        F<IT, OT>{}(std::forward<Args>(args)...);
      }
    };

    template <typename ...Args>
    void operator()(DataType output_type, Args... args) const { 
      dispatch<OutputType>(output_type, std::forward<Args>(args)...);
    }
  };
  
  template <typename ...Args>
  void operator()(DataType input_data_type, DataType output_data_type, Args... args) {
    dispatch<InputType>(input_data_type, output_data_type, std::forward<Args>(args)...);
  }
};

}

#endif
