#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

#include <QPropertyAnimation>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qorbitcameracontroller.h>

#include <QFile>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(QColor::fromRgbF(0.50, 0.5, 0.50, 1.0));

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *cameraEntity = view.camera();

    cameraEntity->lens()->setPerspectiveProjection(90.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(-16.0f, 0.0f, 40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(-1, 0, -1));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);

    // Torus
    Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity(rootEntity);

    // Transform
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setScale(8.0f);

    // Custom Mesh (TetraHedron)
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ///
        QVector3D vxx[36];
        QVector3D nxx[12];
        vxx[0].setX(-1.0f);//-1.0f, 0.0f, -0.0f
        vxx[0].setY(0.0f);
        vxx[0].setZ(-1.0f);
        vxx[1].setX(-1.0f);//1.0f, 0.0f, -0.0f
        vxx[1].setY(0.0f);
        vxx[1].setZ(0.50f);
        vxx[2].setX(-1.0f);//0.0f, 1.0f, 0.0f
        vxx[2].setY(0.50f);
        vxx[2].setZ(-1.0f);
        vxx[3].setX(0.0f);//(0.0f, 0.5f, 0.0f);
        vxx[3].setY(0.5f);
        vxx[3].setZ(0.0f);


        QString strFilename = "/Users/oliviermanette/QtApps/3dtest1/3dtest1.stl";
        QFile file(strFilename);
        QByteArray gbaBuffer;

        if (file.open(QIODevice::ReadOnly))
            gbaBuffer = file.readAll();
        file.close();

        uint j=0;
        uint luintIdx=0;
        uint luintSize = *reinterpret_cast<uint*>((gbaBuffer.data()+80));
        qDebug()<<"FLOD3D: The number of triangles is: "<<luintSize;
        float lvector;
        for (uint i=0;i<9;i+=3){
            lvector = *reinterpret_cast<float*>((gbaBuffer.data()+96+50*j+i*sizeof (float)));
            qDebug()<<"FLOD3D: vertex ("<<luintIdx<<") :"<<lvector;
            vxx[luintIdx].setX(lvector);//-1.0f, 0.0f, -0.0f
            lvector = *reinterpret_cast<float*>((gbaBuffer.data()+96+50*j+i*sizeof (float)+1*sizeof (float)));
            qDebug()<<"FLOD3D: vertex ("<<luintIdx<<") :"<<lvector;
            vxx[luintIdx].setY(lvector);
            lvector = *reinterpret_cast<float*>((gbaBuffer.data()+96+50*j+i*sizeof (float)+2*sizeof (float)));
            qDebug()<<"FLOD3D: vertex ("<<luintIdx<<") :"<<lvector;
            vxx[luintIdx].setZ(lvector);

            luintIdx++;
        }
        qDebug()<<"dataSize before : "<<gbaBuffer.size();
        gbaBuffer.remove(0,84);

        for (uint i=0;i<luintSize;i++){
            gbaBuffer.remove(i*(9*sizeof (float)),3*sizeof(float));
            gbaBuffer.remove((i+1)*(9*sizeof (float)),2);
        }
        qDebug()<<"dataSize after : "<<gbaBuffer.size();



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // vec3 for position
    // vec3 for colors
    // vec3 for normals

    /*          2
               /|\
              / | \
             / /3\ \
             0/___\ 1
    */

    // 4 distinct vertices
    QByteArray vertexBufferData;
    vertexBufferData.resize(4 * (3 + 3 + 3) * sizeof(float));

    // Faces Normals
    QVector3D n023 = QVector3D::normal(vxx[0], vxx[2], vxx[3]);
    QVector3D n012 = QVector3D::normal(vxx[0], vxx[1], vxx[2]);
    QVector3D n310 = QVector3D::normal(vxx[3], vxx[1], vxx[0]);
    QVector3D n132 = QVector3D::normal(vxx[1], vxx[3], vxx[2]);

    // Vector Normals
    QVector3D n0 = QVector3D(n023 + n012 + n310).normalized();
    QVector3D n1 = QVector3D(n132 + n012 + n310).normalized();
    QVector3D n2 = QVector3D(n132 + n012 + n023).normalized();
    QVector3D n3 = QVector3D(n132 + n310 + n023).normalized();

    // Colors
    QVector3D red(1.0f, 0.0f, 0.0f);
    QVector3D green(0.0f, 1.0f, 0.0f);
    QVector3D blue(0.0f, 0.0f, 1.0f);
    QVector3D white(1.0f, 1.0f, 1.0f);
    QVector3D yellow(1.0f, 1.0f, 0.0f);
    QVector3D orange(0.99f, 0.415f, 0.007f);

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << vxx[0] << n0 << yellow
            << vxx[1] << n1 << blue
            << vxx[2] << n2 << red;
     //       << vxx[3] << n3 << red;

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;

    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }


    qDebug()<<"idx: "<<idx;


    // Indices (12)
    QByteArray indexBufferData;
    indexBufferData.resize(4 * 3 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    // Front
    rawIndexArray[0] = 0;
    rawIndexArray[1] = 1;
    rawIndexArray[2] = 2;
    // Bottom
    rawIndexArray[3] = 3;
    rawIndexArray[4] = 1;
    rawIndexArray[5] = 0;
    // Left
    rawIndexArray[6] = 0;
    rawIndexArray[7] = 2;
    rawIndexArray[8] = 3;
    // Right
    rawIndexArray[9] = 1;
    rawIndexArray[10] = 3;
    rawIndexArray[11] = 2;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    vertexDataBuffer->setData(gbaBuffer);
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3*sizeof (float));
    positionAttribute->setCount(luintSize);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
/*
    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexDataBuffer);
    normalAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(50);
    normalAttribute->setCount(12);
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorAttribute->setVertexSize(9);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(50);
    colorAttribute->setCount(36);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setVertexSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(1);
*/
    customGeometry->addAttribute(positionAttribute);
    //customGeometry->addAttribute(normalAttribute);
    //customGeometry->addAttribute(colorAttribute);
    //customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setInstanceCount(12);
    customMeshRenderer->setIndexOffset(0);
    customMeshRenderer->setFirstInstance(0);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);
    // 4 faces of 3 points
    customMeshRenderer->setVertexCount(36);

    customMeshEntity->addComponent(customMeshRenderer);
    customMeshEntity->addComponent(transform);
    customMeshEntity->addComponent(material);

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
