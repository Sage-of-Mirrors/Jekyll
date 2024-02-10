#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>


struct J3DTexture;

namespace bStream {
    class CStream;
}

class MTextureData {
    std::vector<std::weak_ptr<J3DTexture>> mTextures;
    std::weak_ptr<J3DTexture> mSelectedTexture;

public:
    MTextureData();
    ~MTextureData();

    void SetTextureData(std::vector<std::weak_ptr<J3DTexture>>& textures) { mTextures = textures; }
    const std::vector<std::weak_ptr<J3DTexture>>& GetTextures() const { return mTextures; }

    void SaveTextureData(bStream::CStream& stream);

    void Clear();
};
