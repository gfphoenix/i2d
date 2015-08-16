/*
 * Main.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: wuhao
 */

#include <App.hpp>
#include <Node.hpp>
#include <Scene.hpp>
#include <iostream>
#include <stdio.h>
#include <VertexUVColor.hpp>
#include <mm.hpp>

using namespace std;
//using namespace fphoenix;
void dumpChild(Node *n)
{
    auto &children = n->getChildren();
    for(auto ch : children){
        printf("Node - %s, zIndex=%d\n", ch->getName().c_str(), ch->getZindex());
    }
}
int mainx()
{
	Scene node;
    node.setName("Scene");
	//std::cout<<node.getModelMat()<<endl;
	auto &&m = node.getWorldTransform();
    for(auto i=0; i<4; i++){
        for(auto j=0; j<4; j++)
            printf("%f ", m[j][i]);
        printf("%s", "\n");
    }
    auto n1 = MM<Node>::New();
    auto n2 = MM<Node>::New();
    //auto n1 = New(Node);
    //auto n2 = New(Node);
    n1->setName("N1");
    n2->setName("N2");
    n1->setZindex(4);
    std::cout<<"n1's name = "<< n1->getName() << std::endl;
    std::cout<<"n2's name = "<< n2->getName() << std::endl;
    //node.addChild(Ref_ptr<Node>(n1));
    //node.addChild(Ref_ptr<Node>(n2));
    node.addChild((n1));
    node.addChild((n2));

    dumpChild(&node);
    node.sortChildrenOrder__();
    dumpChild(&node);

    node.Render();

    //node.removeAll();
    node.removeChild(n1);
    node.removeChild(n2);

    //node.setColor(255,255,255,128);
    node.setColor(Color4(.1, .2, .3, .4));
    //Vec4 color(255,255,255,128);
    auto const &color = node.getColor();
    printf("rgba = %f, %f, %f, %f\n", color.r, color.g, color.b, color.a);

    Mat3 a={{0,1,2,},{3,4,5,},{6,7,8}};
    Mat4 b(a);
    for(auto i=0; i<4; i++){
        for(auto j=0; j<4; j++)
            printf("%f ", b[j][i]);
        printf("%s", "\n");
    }


    V224_Quad v2;
    V324_Quad v3;
    printf("size v224=%d, v324=%d, float=%d\n",
            sizeof(v2), sizeof(v3), sizeof(float));

	return 0;
}

int main()
{
    auto app = App::getInstance();
    app->go();

    return 0;
}
