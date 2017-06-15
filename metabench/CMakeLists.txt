if (NOT EXISTS "/opt/metabench.cmake")
  return()
endif()

include("/opt/metabench.cmake")

function(nbdl_add_metabench target range)
  metabench_add_dataset("metabench.${target}_dataset" "${target}.cpp" "${range}"
    NAME "${target}"
  )

  metabench_add_chart("metabench.${target}"
    ASPECT COMPILATION_TIME
    TITLE "${target}"
    SUBTITLE "Compile Time"
    DATASETS "metabench.${target}_dataset"
  )

  metabench_add_chart("metabench.${target}_file_size"
    ASPECT EXECUTABLE_SIZE
    TITLE "${target}"
    SUBTITLE "Link Time"
    DATASETS "metabench.${target}_dataset"
  )
endfunction()

nbdl_add_metabench(params_promise   "(0..50).step(10)")
nbdl_add_metabench(detail_match_if  "(0..50).step(10)")
nbdl_add_metabench(run_sync         "(0..50).step(10)")
nbdl_add_metabench(variant          "(0..50).step(10)")
nbdl_add_metabench(variant_match    "(0..50).step(10)")
