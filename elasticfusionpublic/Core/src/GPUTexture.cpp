/*
 * This file is part of ElasticFusion.
 *
 * Copyright (C) 2015 Imperial College London
 * 
 * The use of the code within this file and all code within files that 
 * make up the software that is ElasticFusion is permitted for 
 * non-commercial purposes only.  The full terms and conditions that 
 * apply to the code within this file are detailed within the LICENSE.txt 
 * file and at <http://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/elastic-fusion-license/> 
 * unless explicitly stated.  By downloading this file you agree to 
 * comply with these terms.
 *
 * If you wish to use any of this code for commercial purposes then 
 * please email researchcontracts.engineering@imperial.ac.uk.
 *
 */
 
#include "GPUTexture.h"
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

const std::string GPUTexture::RGB = "RGB";
const std::string GPUTexture::DEPTH_RAW = "DEPTH";
const std::string GPUTexture::DEPTH_FILTERED = "DEPTH_FILTERED";
const std::string GPUTexture::DEPTH_METRIC = "DEPTH_METRIC";
const std::string GPUTexture::DEPTH_METRIC_FILTERED = "DEPTH_METRIC_FILTERED";
const std::string GPUTexture::DEPTH_NORM = "DEPTH_NORM";

const std::string GPUTexture::INSTANCE_GROUNDTRUTH = "INSTANCE_GROUNDTRUTH";

GPUTexture::GPUTexture(const int width,
                       const int height,
                       const GLenum internalFormat,
                       const GLenum format,
                       const GLenum dataType,
                       const bool draw,
                       const bool cuda)
 : texture(new pangolin::GlTexture(width, height, internalFormat, draw, 0, format, dataType)),
   draw(draw),
   width(width),
   height(height),
   internalFormat(internalFormat),
   format(format),
   dataType(dataType)
{
    if(cuda)
    {
        cudaGraphicsGLRegisterImage(&cudaRes, texture->tid, GL_TEXTURE_2D, cudaGraphicsRegisterFlagsReadOnly);
        cudaGraphicsMapResources(1, &cudaRes, 0);
        cudaGraphicsSubResourceGetMappedArray(&cudaArr, cudaRes, 0, 0);
        cudaResourceDesc resDesc;
        cudaTextureDesc texDesc;
        memset(&resDesc, 0, sizeof(cudaResourceDesc));
        memset(&texDesc, 0, sizeof(cudaTextureDesc));
        resDesc.resType = cudaResourceTypeArray;
        resDesc.res.array.array  = cudaArr;
        texDesc.normalizedCoords = false;
        texDesc.addressMode[0] = cudaAddressModeClamp;
        texDesc.addressMode[1] = cudaAddressModeClamp;
        texDesc.filterMode = cudaFilterModePoint;
        texDesc.readMode = cudaReadModeElementType;
        cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL);
        cudaGraphicsUnmapResources(1, &cudaRes, 0);
    }
    else
    {
        cudaRes = 0;
    }
}

GPUTexture::~GPUTexture()
{
    if(texture)
    {
        delete texture;
    }

    if(cudaRes)
    {
        cudaGraphicsUnregisterResource(cudaRes);
    }
}
