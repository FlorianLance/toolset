
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "dc_filters.hpp"

// local
#include "utility/io_data.hpp"

using namespace tool::camera;

auto DCFilters::default_init_for_calibration() -> DCFilters{
    DCFilters filters;
    // TODO
    return filters;
}

auto DCFilters::init_from_data(std::int8_t *data) -> void{

    size_t offset = 0;
    read(minWidth, data, offset, m_inputFileSize);
    read(maxWidth, data, offset, m_inputFileSize);
    read(minHeight, data, offset, m_inputFileSize);
    read(maxHeight, data, offset, m_inputFileSize);
    read(minDepthValue, data, offset, m_inputFileSize);
    read(maxDepthValue, data, offset, m_inputFileSize);
    read(yFactor, data, offset, m_inputFileSize);
    read(uFactor, data, offset, m_inputFileSize);
    read(vFactor, data, offset, m_inputFileSize);
    read(filterDepthWithColor, data, offset, m_inputFileSize);
    read(filterColor, data, offset, m_inputFileSize);
    read(maxDiffColor, data, offset, m_inputFileSize);
    read(jpegCompressionRate, data, offset, m_inputFileSize);
    read(doLocalDiffFiltering, data, offset, m_inputFileSize);
    read(maxLocalDiff, data, offset, m_inputFileSize);
    read(doMinNeighboursFiltering, data, offset, m_inputFileSize);
    read(nbMinNeighbours, data, offset, m_inputFileSize);
    read(minNeighboursLoops, data, offset, m_inputFileSize);
    read(doErosion, data, offset, m_inputFileSize);
    read(erosionLoops, data, offset, m_inputFileSize);
    read(keepOnlyBiggestCluster, data, offset, m_inputFileSize);
    read(invalidateColorFromDepth, data, offset, m_inputFileSize);
    read(invalidateInfraFromDepth, data, offset, m_inputFileSize);
    read(p1FMode, data, offset, m_inputFileSize);
    read(p1Pos, data, offset, m_inputFileSize);
    read(p1Rot, data, offset, m_inputFileSize);
    read(p2FMode, data, offset, m_inputFileSize);
    read(p2Pos, data, offset, m_inputFileSize);
    read(p2Rot, data, offset, m_inputFileSize);
}

auto DCFilters::convert_to_data(std::int8_t *data) const -> void{

    size_t offset = 0;
    write(minWidth, data, offset);
    write(maxWidth, data, offset);
    write(minHeight, data, offset);
    write(maxHeight, data, offset);
    write(minDepthValue, data, offset);
    write(maxDepthValue, data, offset);
    write(yFactor, data, offset);
    write(uFactor, data, offset);
    write(vFactor, data, offset);
    write(filterDepthWithColor, data, offset);
    write(filterColor, data, offset);
    write(maxDiffColor, data, offset);
    write(jpegCompressionRate, data, offset);
    write(doLocalDiffFiltering, data, offset);
    write(maxLocalDiff, data, offset);
    write(doMinNeighboursFiltering, data, offset);
    write(nbMinNeighbours, data, offset);
    write(minNeighboursLoops, data, offset);
    write(doErosion, data, offset);
    write(erosionLoops, data, offset);
    write(keepOnlyBiggestCluster, data, offset);
    write(invalidateColorFromDepth, data, offset);
    write(invalidateInfraFromDepth, data, offset);
    write(p1FMode, data, offset);
    write(p1Pos, data, offset);
    write(p1Rot, data, offset);
    write(p2FMode, data, offset);
    write(p2Pos, data, offset);
    write(p2Rot, data, offset);
}
