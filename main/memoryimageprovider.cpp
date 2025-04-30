#include "memoryimageprovider.h"

QImage MemoryImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    (void)requestedSize;

    if (m_mainController == nullptr)
    {
        return QImage();
    }

    QImage* retImage = nullptr;
    if (id.indexOf("avatar") == 0)
    {
        QStringList parts = id.split("_");
        QImage avatarImg = m_mainController->getAvatarImg(parts[1]);
        retImage = &avatarImg;
    }

    if (retImage)
    {
        if (size)
        {
            *size = retImage->size();
        }
        return *retImage;
    }

    return QImage();
}
