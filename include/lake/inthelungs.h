#pragma once

#include <lake/bedrock/bedrock.h>

#include <lake/data_structures/arena_allocator.h>
#include <lake/data_structures/bitset.h>
#include <lake/data_structures/block_allocator.h>
#include <lake/data_structures/dagraph.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/deque.h>
#include <lake/data_structures/hashmap.h>
#include <lake/data_structures/map.h>
#include <lake/data_structures/mpmc_ring.h>
#include <lake/data_structures/sparse_set.h>
#include <lake/data_structures/stack_allocator.h>
#include <lake/data_structures/strbuf.h>
#include <lake/data_structures/switch_list.h>

#include <lake/math/bits.h>
#include <lake/math/camera.h>
#include <lake/math/matrix.h>
#include <lake/math/quaternion.h>
#include <lake/math/trigonometry.h>
#include <lake/math/vector.h>

#include <lake/modules/hadal.h>
#include <lake/modules/hadean.h>
#include <lake/modules/ipomoea.h>
#include <lake/modules/lovage.h>
#include <lake/modules/moon.h>
#include <lake/modules/riven.h>
#include <lake/modules/soma.h>
#include <lake/modules/sorceress.h>
#include <lake/modules/volta.h>

#include <lake/audio/dsp.h>
#include <lake/audio/mixer.h>
#include <lake/audio/spatial.h>
#include <lake/audio/synth.h>

#include <lake/graphics/pipeline_builder.h>
#include <lake/graphics/render_graph.h>
#include <lake/graphics/renderer.h>

#ifdef LAKE_IN_THE_LUNGS_MAIN
#include <lake/bedrock/private/main_impl.h>
#endif
