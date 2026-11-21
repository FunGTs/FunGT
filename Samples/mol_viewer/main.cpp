#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main()
{
    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myScene = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myScene->setBackgroundColor();
    myScene->initGL();

    FunGTSceneManager scene_manager = myScene->getSceneManager();

    FunGTMoleculeViewer mol = MoleculeViewer::create("Samples/mol_viewer/mol.xyz", fungt::MC_MODE::GPU);
    mol->setResolution(160, 160, 160);
    mol->load();

    myScene->set([&]() {
        scene_manager->addRenderableObj(mol);
    });

    myScene->render([&]() {
    });

    return 0;
}
