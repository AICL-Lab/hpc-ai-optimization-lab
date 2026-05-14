<script setup lang="ts">
import { ref, computed } from 'vue'

interface Module {
  id: string
  name: string
  nameZh: string
  description: string
  descriptionZh: string
  link: string
  x: number
  y: number
  color: string
}

const props = defineProps<{
  lang?: 'en' | 'zh-CN'
}>()

const lang = computed(() => props.lang || 'en')

const modules: Module[] = [
  {
    id: 'tensor-core',
    name: 'Tensor Core',
    nameZh: 'Tensor Core',
    description: 'WMMA/MMA PTX instructions, 70+ TFLOPS',
    descriptionZh: 'WMMA/MMA PTX 指令，70+ TFLOPS',
    link: '/en/guide/gemm',
    x: 300,
    y: 40,
    color: '#8b5cf6'
  },
  {
    id: 'gemm',
    name: 'GEMM',
    nameZh: 'GEMM',
    description: '7-step optimization journey from naive to Tensor Core',
    descriptionZh: '从朴素实现到 Tensor Core 的 7 步优化之旅',
    link: '/en/guide/gemm',
    x: 150,
    y: 140,
    color: '#3b82f6'
  },
  {
    id: 'attention',
    name: 'Attention',
    nameZh: 'Attention',
    description: 'FlashAttention with memory-efficient implementation',
    descriptionZh: 'FlashAttention 内存高效实现',
    link: '/en/guide/flash-attention',
    x: 300,
    y: 140,
    color: '#8b5cf6'
  },
  {
    id: 'quant',
    name: 'Quantization',
    nameZh: '量化',
    description: 'INT8/FP8 quantization for inference acceleration',
    descriptionZh: 'INT8/FP8 量化加速推理',
    link: '/en/api/index',
    x: 450,
    y: 140,
    color: '#f59e0b'
  },
  {
    id: 'memory',
    name: 'Memory',
    nameZh: '访存优化',
    description: 'Shared memory tiling, coalesced access patterns',
    descriptionZh: '共享内存分块、合并访存模式',
    link: '/en/guide/memory',
    x: 150,
    y: 240,
    color: '#06b6d4'
  },
  {
    id: 'reduction',
    name: 'Reduction',
    nameZh: '归约',
    description: 'Parallel reduction, warp-level primitives',
    descriptionZh: '并行归约、warp 级原语',
    link: '/en/guide/reduction',
    x: 300,
    y: 240,
    color: '#06b6d4'
  },
  {
    id: 'elementwise',
    name: 'Elementwise',
    nameZh: '逐元素',
    description: 'ReLU, Sigmoid, Transpose operations',
    descriptionZh: 'ReLU、Sigmoid、转置操作',
    link: '/en/api/index',
    x: 450,
    y: 240,
    color: '#64748b'
  },
  {
    id: 'cuda-core',
    name: 'CUDA Core',
    nameZh: 'CUDA 核心',
    description: 'Base CUDA primitives and utilities',
    descriptionZh: '基础 CUDA 原语与工具',
    link: '/en/api/architecture',
    x: 300,
    y: 340,
    color: '#1a365d'
  }
]

const connections = [
  { from: 'cuda-core', to: 'memory', type: 'solid' },
  { from: 'cuda-core', to: 'reduction', type: 'solid' },
  { from: 'cuda-core', to: 'elementwise', type: 'solid' },
  { from: 'memory', to: 'gemm', type: 'solid' },
  { from: 'reduction', to: 'attention', type: 'solid' },
  { from: 'elementwise', to: 'quant', type: 'solid' },
  { from: 'gemm', to: 'tensor-core', type: 'solid' },
  { from: 'attention', to: 'tensor-core', type: 'dashed' },
  { from: 'quant', to: 'tensor-core', type: 'dashed' }
]

const hoveredModule = ref<string | null>(null)
const tooltipPos = ref({ x: 0, y: 0 })

const handleMouseEnter = (module: Module, event: MouseEvent) => {
  hoveredModule.value = module.id
  const rect = (event.target as HTMLElement).getBoundingClientRect()
  tooltipPos.value = { x: rect.left + rect.width / 2, y: rect.top - 10 }
}

const handleMouseLeave = () => {
  hoveredModule.value = null
}

const getModuleById = (id: string) => modules.find(m => m.id === id)

const getName = (module: Module) => lang.value === 'zh-CN' ? module.nameZh : module.name
const getDescription = (module: Module) => lang.value === 'zh-CN' ? module.descriptionZh : module.description
</script>

<template>
  <div class="architecture-diagram">
    <h3 class="diagram-title">
      {{ lang === 'zh-CN' ? '系统架构' : 'System Architecture' }}
    </h3>

    <div class="diagram-container">
      <svg viewBox="0 0 600 400" class="diagram-svg">
        <!-- Connections -->
        <g class="connections">
          <template v-for="conn in connections" :key="`${conn.from}-${conn.to}`">
            <line
              :x1="getModuleById(conn.from)?.x"
              :y1="getModuleById(conn.from)?.y"
              :x2="getModuleById(conn.to)?.x"
              :y2="getModuleById(conn.to)?.y"
              :stroke="getModuleById(conn.from)?.color"
              :stroke-dasharray="conn.type === 'dashed' ? '5,5' : 'none'"
              stroke-width="2"
              opacity="0.6"
              class="connection-line"
            />
          </template>
        </g>

        <!-- Modules -->
        <g class="modules">
          <g
            v-for="module in modules"
            :key="module.id"
            :transform="`translate(${module.x}, ${module.y})`"
            class="module-group"
            @mouseenter="handleMouseEnter(module, $event)"
            @mouseleave="handleMouseLeave"
          >
            <!-- Module circle -->
            <circle
              r="40"
              :fill="module.color"
              :class="{ 'module-hovered': hoveredModule === module.id }"
              class="module-circle"
            />
            <!-- Module label -->
            <text
              text-anchor="middle"
              dy="5"
              fill="white"
              font-size="12"
              font-weight="600"
              class="module-text"
            >
              {{ getName(module) }}
            </text>
          </g>
        </g>

        <!-- Data flow animation -->
        <g class="data-flow">
          <circle r="4" fill="#4fd1c5" class="flow-particle">
            <animateMotion
              dur="3s"
              repeatCount="indefinite"
              path="M300,340 L150,240 L150,140 L300,40"
            />
          </circle>
          <circle r="4" fill="#4fd1c5" class="flow-particle">
            <animateMotion
              dur="3s"
              repeatCount="indefinite"
              begin="1s"
              path="M300,340 L300,240 L300,140 L300,40"
            />
          </circle>
        </g>
      </svg>

      <!-- Tooltip -->
      <Transition name="tooltip">
        <div
          v-if="hoveredModule"
          class="tooltip"
          :style="{ left: tooltipPos.x + 'px', top: tooltipPos.y + 'px' }"
        >
          <div class="tooltip-content">
            {{ getDescription(getModuleById(hoveredModule)!) }}
          </div>
        </div>
      </Transition>
    </div>

    <!-- Legend -->
    <div class="legend">
      <div class="legend-item">
        <span class="legend-line solid"></span>
        <span>{{ lang === 'zh-CN' ? '直接依赖' : 'Direct dependency' }}</span>
      </div>
      <div class="legend-item">
        <span class="legend-line dashed"></span>
        <span>{{ lang === 'zh-CN' ? '可选扩展' : 'Optional extension' }}</span>
      </div>
      <div class="legend-item">
        <span class="legend-dot"></span>
        <span>{{ lang === 'zh-CN' ? '数据流' : 'Data flow' }}</span>
      </div>
    </div>
  </div>
</template>

<style scoped>
.architecture-diagram {
  margin: 32px 0;
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border-radius: 16px;
  border: 1px solid var(--vp-c-divider);
}

.diagram-title {
  text-align: center;
  margin: 0 0 24px;
  font-size: 1.25rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
}

.diagram-container {
  position: relative;
  width: 100%;
  max-width: 600px;
  margin: 0 auto;
}

.diagram-svg {
  width: 100%;
  height: auto;
}

.module-circle {
  cursor: pointer;
  transition: all 0.3s ease;
  filter: drop-shadow(0 4px 8px rgba(0, 0, 0, 0.2));
}

.module-circle:hover,
.module-circle.module-hovered {
  r: 45;
  filter: drop-shadow(0 6px 12px rgba(0, 0, 0, 0.3));
}

.module-text {
  pointer-events: none;
  user-select: none;
}

.connection-line {
  transition: opacity 0.3s ease;
}

.flow-particle {
  filter: drop-shadow(0 0 4px currentColor);
}

.tooltip {
  position: fixed;
  transform: translate(-50%, -100%);
  z-index: 100;
  pointer-events: none;
}

.tooltip-content {
  background: var(--vp-c-bg-elv);
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  padding: 8px 12px;
  font-size: 0.875rem;
  color: var(--vp-c-text-2);
  white-space: nowrap;
  box-shadow: var(--vp-shadow-3);
}

.tooltip-enter-active,
.tooltip-leave-active {
  transition: all 0.2s ease;
}

.tooltip-enter-from,
.tooltip-leave-to {
  opacity: 0;
  transform: translate(-50%, -90%);
}

.legend {
  display: flex;
  justify-content: center;
  gap: 24px;
  margin-top: 24px;
  padding-top: 16px;
  border-top: 1px solid var(--vp-c-divider);
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 0.8125rem;
  color: var(--vp-c-text-2);
}

.legend-line {
  width: 24px;
  height: 2px;
  background: var(--vp-c-brand-1);
}

.legend-line.dashed {
  background: repeating-linear-gradient(
    90deg,
    var(--vp-c-brand-1) 0px,
    var(--vp-c-brand-1) 5px,
    transparent 5px,
    transparent 10px
  );
}

.legend-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: #4fd1c5;
}

@media (max-width: 640px) {
  .legend {
    flex-direction: column;
    gap: 12px;
    align-items: center;
  }
}
</style>
