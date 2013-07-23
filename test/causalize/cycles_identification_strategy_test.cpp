//  (C) Copyright Gennadiy Rozental 2002-2006.
//  (C) Copyright Gennadiy Rozental & Ullrich Koethe 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

#include <causalize/cycles_identification_strategy.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp> 
using namespace boost::unit_test;

//____________________________________________________________________________//

void cycles_num_test()
{
    CausalizationGraph graph;

    VertexProperties vp1;
    vp1.type = E;
    Vertex e1 = add_vertex(vp1, graph);
    VertexProperties vp2;
    vp2.type = U;
    Vertex u1 = add_vertex(vp2, graph);
    VertexProperties vp3;
    vp3.type = E;
    Vertex e2 = add_vertex(vp3, graph);
    VertexProperties vp4;
    vp4.type = U;
    Vertex u2 = add_vertex(vp4, graph);
    VertexProperties vp5;
    vp5.type = E;
    Vertex e3 = add_vertex(vp5, graph);
    VertexProperties vp6;
    vp6.type = U;
    Vertex u3 = add_vertex(vp6, graph);

    add_edge(e1, u1, graph);
    add_edge(e2, u2, graph);
    add_edge(e3, u3, graph);
    add_edge(e2, u3, graph);
    add_edge(e3, u2, graph);

    std::map<Vertex, int> *cycles = new std::map<Vertex, int>;

    BOOST_CHECK(cycles_identification_strategy(graph, cycles) == 1);
}

//____________________________________________________________________________//


test_suite*
init_unit_test_suite( int, char* [] ) {
    framework::master_test_suite().p_name.value = "Cycles Identification Strategy";

    framework::master_test_suite().add( BOOST_TEST_CASE( &cycles_num_test ));

    return 0;
}

//____________________________________________________________________________//

// EOF



