#include "memoryimageprovider.h"
#include "../Utility/ImPath.h"

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
    else if (id == "qrcode")
    {
        static QImage qrCode;
        if (qrCode.isNull())
        {
            QString qrcodeFilePath = QString::fromStdWString(CImPath::GetConfPath()) + "qrcode.png";
            qrCode.load(qrcodeFilePath);
        }
        retImage = &qrCode;
    }
    else if (id == "advertise")
    {
        static QImage advertise;
        if (advertise.isNull())
        {
            QString advertiseFilePath = QString::fromStdWString(CImPath::GetConfPath()) + "advertise.jpg";
            advertise.load(advertiseFilePath);
        }
        retImage = &advertise;
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
