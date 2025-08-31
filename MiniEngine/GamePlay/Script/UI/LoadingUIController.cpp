#include "LoadingUIController.h"

#include "Engine/Application.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/Font/TankinFont.h"
#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(LoadingUIController, "LoadingUIController")

void LoadingUIController::awake()
{
    mLoadingImage = dynamic_cast<ImageTGUI*>(
        mGameObject->getTransform()->getChildWithName("LoadingProgress")->getGameObject()->getComponent("ImageTGUI"));
    
    mLoadingImageTransform = dynamic_cast<RectTransform*>(mLoadingImage->getGameObject()->getTransform());
    mLoadingTagTransform = mGameObject->getTransform()->getChildWithName("LoadingTag");
    mLogoImage =
        dynamic_cast<ImageTGUI*>(mGameObject->getTransform()->getChildWithName("Logo")->getGameObject()->getComponent("ImageTGUI"));

    mLogoImage->getGameObject()->activeGameObject();
    mLoadingTagTransform->getGameObject()->deactiveGameObject();
    mLoadingImageTransform->getGameObject()->deactiveGameObject();
    mLogoImage->setBlendFactor(1);
    isFirstShow = 0;
}

void LoadingUIController::start()
{
    
    
}

void LoadingUIController::startFileLoad()
{
    //start时开始异步加载
    mCurrentProgress = 0;
    mLoadingIsDone = 0;
    mLoadingImageTransform->setWidth(0);

    tasks.clear();
    rapidxml::xml_node<>* fileManagerNode = Scene::sGetFileManagerNode();
    ASSERT(fileManagerNode, TEXT("fileManagerNode is null!"))

    //开始多线程加载
    FileManager::deserializeSelf(fileManagerNode, tasks);

    //开启一个线程用于等待加载完成
    auto threadPool = Application::sGetThreadPool();
    threadPool->enqueue(0,[this]()
    {
        //等待加载完毕
        int i = 0;
        size_t size = tasks.size();
        for (auto& task:tasks)
        {
            task.wait();
            ++i;
            mCurrentProgress = static_cast<float>(i)/size;
        }
        mLoadingIsDone = 1;
    });
}

void LoadingUIController::update()
{
    if (isFirstShow < 100)
    {
        static float alphaSpeed = -0.01f;
        if (mLogoImage->getBlendFactor() > 0.01f)
        {
            mLogoImage->setBlendFactor(mLogoImage->getBlendFactor() + alphaSpeed);
        }
        else
        {
            isFirstShow++;
        }
    }
    else if (isFirstShow == 100)
    {
        static float alphaSpeed = 0.01f;
        if (mLogoImage->getBlendFactor() < 1)
        {
            mLogoImage->setBlendFactor(mLogoImage->getBlendFactor() + alphaSpeed);
        }
        else
        {
            isFirstShow = 101;
            startFileLoad();
            mLogoImage->getGameObject()->deactiveGameObject();
            mLoadingTagTransform->getGameObject()->activeGameObject();
            mLoadingImageTransform->getGameObject()->activeGameObject();
        }
    }
    else
    {
        float rotateSpeed = 100;
        mLoadingTagTransform->rotateLocalPitchYawRoll({
            0,
            0,
            static_cast<float>(rotateSpeed*GameTime::sGetDeltaTime()*MathUtils::DEG_TO_RAD)});
    
        if (mLoadingIsDone == 1)
        {
            mLoadingImageTransform->setWidth(mWidth*mCurrentProgress);
            //先提交所有Asset
            FileManager::uploadAllAssets();
        
            //加载结束后，解析字体
            //deSerialize Fonts
            TankinFont* fonts = TankinFont::sGetInstance();
            ++mLoadingIsDone;
        }
        else if (mLoadingIsDone == 30)
        {
            //延迟几帧再切换场景
            Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/StartScene.xml");
        }
        else if (mLoadingIsDone > 0)
        {
            ++mLoadingIsDone;
        }
        else if (mLoadingIsDone == 0)
        {
            //更新加载进度
            mLoadingImageTransform->setWidth(mWidth*mCurrentProgress);
        
        }
    }
}


