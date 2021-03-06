
#ifndef TESTCAPSULESIMULATION2D_HPP_
#define TESTCAPSULESIMULATION2D_HPP_

#include <cxxtest/TestSuite.h>
#include <cycle/UniformCellCycleModel.hpp>

// Must be included before any other cell_based headers
#include "CellBasedSimulationArchiver.hpp"

#include "AbstractCellBasedTestSuite.hpp"
#include "CellsGenerator.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "OffLatticeSimulation.hpp"
#include "NoCellCycleModel.hpp"
#include "NodesOnlyMesh.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "Cell.hpp"
#include "RandomNumberGenerator.hpp"
#include "UblasCustomFunctions.hpp"
#include "UniformCellCycleModel.hpp"
#include "TransitCellProliferativeType.hpp"
#include "CellIdWriter.hpp"

// Header files included in this project
#include "TypeSixSecretionEnumerations.hpp"
#include "ForwardEulerNumericalMethodForCapsules.hpp"
#include "CapsuleForce.hpp"
#include "CapsuleOrientationWriter.hpp"
#include "CapsuleScalingWriter.hpp"
#include "SquareBoundaryCondition.hpp"
#include "CapsuleBasedDivisionRule.hpp"
#include "TypeSixMachineModifier.hpp"
#include "NodeBasedCellPopulationWithCapsules.hpp"
#include "TypeSixMachineProperty.hpp"
#include "TypeSixMachineCellKiller.hpp"
#include "MachineStateCountWriter.hpp"



// Should usually be called last.
#include "PetscSetupAndFinalize.hpp"

#include "Debug.hpp"

class TestCapsuleSimulation2d : public AbstractCellBasedTestSuite
{
public:

	void xTestSmallSymmetric2dCapsuleSimulation()
	{
		EXIT_IF_PARALLEL;
		// Create some capsules
		std::vector<Node<2>*> nodes;

		nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));
		nodes.push_back(new Node<2>(1u, Create_c_vector(4.0, 6.0)));
		nodes.push_back(new Node<2>(2u, Create_c_vector(7.0, 4.0)));
		nodes.push_back(new Node<2>(3u, Create_c_vector(7.0, 6.0)));

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */
		NodesOnlyMesh<2> mesh;
		mesh.ConstructNodesWithoutMesh(nodes, 150.5);

		mesh.GetNode(0u)->AddNodeAttribute(0.0);
		mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(1u)->AddNodeAttribute(0.0);
		mesh.GetNode(1u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_THETA] = -0.25 * M_PI;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(2u)->AddNodeAttribute(0.0);
		mesh.GetNode(2u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_THETA] = -0.25 * M_PI;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(3u)->AddNodeAttribute(0.0);
		mesh.GetNode(3u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		//Create cells
		std::vector<CellPtr> cells;
		auto p_diff_type = boost::make_shared<DifferentiatedCellProliferativeType>();
		CellsGenerator<NoCellCycleModel, 2> cells_generator;
		cells_generator.GenerateBasicRandom(cells, mesh.GetNumNodes(), p_diff_type);

		// Create cell population
		NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

		population.AddCellWriter<CellIdWriter>();
		population.AddCellWriter<CapsuleOrientationWriter>();
		population.AddCellWriter<CapsuleScalingWriter>();

		// Create simulation
		OffLatticeSimulation<2> simulator(population);
		simulator.SetOutputDirectory("TestSmallSymmetric2dCapsuleSimulation");
		simulator.SetDt(1.0/1200.0);
		simulator.SetSamplingTimestepMultiple(1u);

		auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
		simulator.SetNumericalMethod(p_numerical_method);

		/*
		 * We now create a force law and pass it to the simulation
		 * We use linear springs between cells up to a maximum of 1.5 ('relaxed' cell diameters) apart, and add this to the simulation class.
		 */
		auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
		simulator.AddForce(p_capsule_force);

		/* We then set an end time and run the simulation */
		simulator.SetEndTime(100.0/1200.0);
		simulator.Solve();
	}



	void TestSmallSymmetric2dCapsuleSimulationWithMachineProperties()
	{
		EXIT_IF_PARALLEL;
		// Create some capsules
		std::vector<Node<2>*> nodes;

		nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));
		nodes.push_back(new Node<2>(1u, Create_c_vector(4.0, 6.0)));

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */
		NodesOnlyMesh<2> mesh;
		mesh.ConstructNodesWithoutMesh(nodes, 150.5);

		mesh.GetNode(0u)->AddNodeAttribute(0.0);
		mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(1u)->AddNodeAttribute(0.0);
		mesh.GetNode(1u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_THETA] = -0.25 * M_PI;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(2u)->AddNodeAttribute(0.0);
		mesh.GetNode(2u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_THETA] = -0.25 * M_PI;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_PHI] = 0.5 * M_PI;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;



		//Create cells
		 // Create cells
		std::vector<CellPtr> cells;
		MAKE_PTR(WildTypeCellMutationState, p_state);
		MAKE_PTR(DifferentiatedCellProliferativeType, p_type);
		for (unsigned i=0; i<mesh.GetNumNodes(); i++)
		{
			UniformCellCycleModel* p_model = new UniformCellCycleModel();
			p_model->SetMinCellCycleDuration(1.0);
			p_model->SetMaxCellCycleDuration(1.01);
			CellPtr p_cell(new Cell(p_state, p_model));
			p_cell->SetCellProliferativeType(p_type);

			double angle_1 = 0.0;
			double angle_2 = M_PI;

			std::vector<double> machine_angles1;
			machine_angles1.push_back(angle_1);
			machine_angles1.push_back(angle_2);



			MAKE_PTR(TypeSixMachineProperty, p_property);
			p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(4, machine_angles1));


			p_cell->AddCellProperty(p_property);
			p_cell->SetBirthTime(-0.9);
			mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0 +3.0*p_cell->GetBirthTime()/p_model->GetCellCycleDuration(); ;

			cells.push_back(p_cell);
		}

		// Create cell population
		NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

		population.AddCellWriter<CellIdWriter>();
		population.AddCellWriter<CapsuleOrientationWriter>();
		population.AddCellWriter<CapsuleScalingWriter>();

		// Create simulation
		OffLatticeSimulation<2> simulator(population);
		simulator.SetOutputDirectory("TestSmallSymmetric2dCapsuleSimulationWithMachineProperties");
		simulator.SetDt(1.0/1200.0);
		simulator.SetSamplingTimestepMultiple(1u);

		auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
		simulator.SetNumericalMethod(p_numerical_method);

		/*
		 * We now create a force law and pass it to the simulation
		 * We use linear springs between cells up to a maximum of 1.5 ('relaxed' cell diameters) apart, and add this to the simulation class.
		 */
		auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
		simulator.AddForce(p_capsule_force);

		/* We then set an end time and run the simulation */
		simulator.SetEndTime(100.0/1200.0);
		simulator.Solve();
	}

	void TestSmallSymmetric2dCapsuleSimulationWithMachinePropertiesAndDivision()
	{
		EXIT_IF_PARALLEL;
		// Create some capsules
		std::vector<Node<2>*> nodes;

		nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */
		NodesOnlyMesh<2> mesh;
		mesh.ConstructNodesWithoutMesh(nodes, 150.5);

		mesh.GetNode(0u)->AddNodeAttribute(0.0);
		mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;


		//Create cells
		std::vector<CellPtr> cells;
		MAKE_PTR(WildTypeCellMutationState, p_state);
		MAKE_PTR(TransitCellProliferativeType, p_type);
		for (unsigned i=0; i<mesh.GetNumNodes(); i++)
		{
			UniformCellCycleModel* p_model = new UniformCellCycleModel();
			p_model->SetMinCellCycleDuration(1.0);
			p_model->SetMaxCellCycleDuration(1.01);
			CellPtr p_cell(new Cell(p_state, p_model));
			p_cell->SetCellProliferativeType(p_type);


			p_cell->SetBirthTime(-0.9);
			mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0 +3.0*p_cell->GetBirthTime()/p_model->GetCellCycleDuration(); ;


			double vertical_coordinate = 0.25*(mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH]);
			double azimuthal_coordinate = M_PI ;


			std::vector<double> machine_coordinates;
			machine_coordinates.push_back(vertical_coordinate);
			machine_coordinates.push_back(azimuthal_coordinate);

			MAKE_PTR(TypeSixMachineProperty, p_property);
			p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(4, machine_coordinates));

			p_cell->AddCellProperty(p_property);

			cells.push_back(p_cell);
		}

		// Create cell population
		NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

		population.AddCellWriter<CellIdWriter>();
		population.AddCellWriter<CapsuleOrientationWriter>();
		population.AddCellWriter<CapsuleScalingWriter>();

		boost::shared_ptr<AbstractCentreBasedDivisionRule<2,2> > p_division_rule(new CapsuleBasedDivisionRule<2,2>());
				 population.SetCentreBasedDivisionRule(p_division_rule);

		// Create simulation
		OffLatticeSimulation<2> simulator(population);
		simulator.SetOutputDirectory("TestSmallSymmetric2dCapsuleSimulationWithMachinePropertiesAndDivision");
		simulator.SetDt(1.0/1200.0);
		simulator.SetSamplingTimestepMultiple(30u);

		auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
		simulator.SetNumericalMethod(p_numerical_method);

		/*
		 * We now create a force law and pass it to the simulation
		 * We use linear springs between cells up to a maximum of 1.5 ('relaxed' cell diameters) apart, and add this to the simulation class.
		 */
		auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
		simulator.AddForce(p_capsule_force);

		/* We then set an end time and run the simulation */
		simulator.SetEndTime(1.1);
		simulator.Solve();

		TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),2u);
	}


    void TestMachinesWithModifiersAndDivision()
    {
        EXIT_IF_PARALLEL;
        // Create some capsules
        std::vector<Node<2>*> nodes;

        nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));

        /*
         * We then convert this list of nodes to a `NodesOnlyMesh`,
         * which doesn't do very much apart from keep track of the nodes.
         */
        NodesOnlyMesh<2> mesh;
        mesh.ConstructNodesWithoutMesh(nodes, 150.5);

        mesh.GetNode(0u)->AddNodeAttribute(0.0);
        mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
        mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
        mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
        mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;


        //Create cells
        std::vector<CellPtr> cells;
        MAKE_PTR(WildTypeCellMutationState, p_state);
        MAKE_PTR(TransitCellProliferativeType, p_type);
        for (unsigned i=0; i<mesh.GetNumNodes(); i++)
        {
            UniformCellCycleModel* p_model = new UniformCellCycleModel();
            p_model->SetMinCellCycleDuration(1.0);
            p_model->SetMaxCellCycleDuration(1.01);
            CellPtr p_cell(new Cell(p_state, p_model));
            p_cell->SetCellProliferativeType(p_type);


            p_cell->SetBirthTime(-0.6);
            mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0 +3.0*p_cell->GetBirthTime()/p_model->GetCellCycleDuration(); ;


            double vertical_coordinate = 0.25*(mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH]);
            double azimuthal_coordinate = M_PI ;


            std::vector<double> machine_coordinates;
            machine_coordinates.push_back(vertical_coordinate);
            machine_coordinates.push_back(azimuthal_coordinate);

            MAKE_PTR(TypeSixMachineProperty, p_property);
            p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(1, machine_coordinates));

            p_cell->AddCellProperty(p_property);

            cells.push_back(p_cell);
        }

        // Create cell population
        NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

        population.AddCellWriter<CellIdWriter>();
        population.AddCellWriter<CapsuleOrientationWriter>();
        population.AddCellWriter<CapsuleScalingWriter>();

        boost::shared_ptr<AbstractCentreBasedDivisionRule<2,2> > p_division_rule(new CapsuleBasedDivisionRule<2,2>());
                 population.SetCentreBasedDivisionRule(p_division_rule);

        // Create simulation
        OffLatticeSimulation<2> simulator(population);
        simulator.SetOutputDirectory("TestMachineModifierWithDivision2d");
        simulator.SetDt(1.0/1200.0);
        simulator.SetSamplingTimestepMultiple(30u);

        auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
        simulator.SetNumericalMethod(p_numerical_method);

        /*
         * We now create a force law and pass it to the simulation
         * We use linear springs between cells up to a maximum of 1.5 ('relaxed' cell diameters) apart, and add this to the simulation class.
         */
        auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
        simulator.AddForce(p_capsule_force);

        MAKE_PTR(TypeSixMachineModifier<2>, p_modifier);
        p_modifier->SetOutputDirectory("TestMachineModifierWithDivision2d");
        p_modifier->Setk_1(0.0);
        p_modifier->Setk_5(0.0);
        p_modifier->Setk_2(0.0);


        simulator.AddSimulationModifier(p_modifier);

    	unsigned num_machines=p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation());

    	TS_ASSERT_EQUALS(num_machines,1u);
        TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),1u);


        /* We then set an end time and run the simulation */
        simulator.SetEndTime(0.35);
        simulator.Solve();
    	unsigned num_machines2=p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation());

    	TS_ASSERT_EQUALS(num_machines2,1u);

        TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),1u);

        simulator.SetEndTime(0.5);
        simulator.Solve();
        unsigned num_machines3=p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation());
        TS_ASSERT_EQUALS(num_machines3,1u);
        TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),2u);
    }

    void TestMachinesWithModifiersAndDivisionVis()
       {
           EXIT_IF_PARALLEL;
           // Create some capsules
           std::vector<Node<2>*> nodes;

           nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));

           /*
            * We then convert this list of nodes to a `NodesOnlyMesh`,
            * which doesn't do very much apart from keep track of the nodes.
            */
           NodesOnlyMesh<2> mesh;
           mesh.ConstructNodesWithoutMesh(nodes, 150.5);

           mesh.GetNode(0u)->AddNodeAttribute(0.0);
           mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
           mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.25 * M_PI;
           mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
           mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;


           //Create cells
           std::vector<CellPtr> cells;
           MAKE_PTR(WildTypeCellMutationState, p_state);
           MAKE_PTR(TransitCellProliferativeType, p_type);
           for (unsigned i=0; i<mesh.GetNumNodes(); i++)
           {
               UniformCellCycleModel* p_model = new UniformCellCycleModel();
               p_model->SetMinCellCycleDuration(1.0);
               p_model->SetMaxCellCycleDuration(1.01);
               CellPtr p_cell(new Cell(p_state, p_model));
               p_cell->SetCellProliferativeType(p_type);


               p_cell->SetBirthTime(-0.6);
               mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0 +3.0*p_cell->GetBirthTime()/p_model->GetCellCycleDuration(); ;


               double vertical_coordinate = 0.25*(mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH]);
               double azimuthal_coordinate = M_PI ;


               std::vector<double> machine_coordinates;
               machine_coordinates.push_back(vertical_coordinate);
               machine_coordinates.push_back(azimuthal_coordinate);

               MAKE_PTR(TypeSixMachineProperty, p_property);
               p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(1, machine_coordinates));

               p_cell->AddCellProperty(p_property);

               cells.push_back(p_cell);
           }

           // Create cell population
           NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

           population.AddCellWriter<CellIdWriter>();
           population.AddCellWriter<CapsuleOrientationWriter>();
           population.AddCellWriter<CapsuleScalingWriter>();

           boost::shared_ptr<AbstractCentreBasedDivisionRule<2,2> > p_division_rule(new CapsuleBasedDivisionRule<2,2>());
                    population.SetCentreBasedDivisionRule(p_division_rule);

           // Create simulation
           OffLatticeSimulation<2> simulator(population);
           simulator.SetOutputDirectory("TestMachineModifierWithDivision2dVis");
           simulator.SetDt(1.0/1200.0);
           simulator.SetSamplingTimestepMultiple(30u);

           auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
           simulator.SetNumericalMethod(p_numerical_method);

           /*
            * We now create a force law and pass it to the simulation
            * We use linear springs between cells up to a maximum of 1.5 ('relaxed' cell diameters) apart, and add this to the simulation class.
            */
           auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
           simulator.AddForce(p_capsule_force);

           MAKE_PTR(TypeSixMachineModifier<2>, p_modifier);
           p_modifier->SetOutputDirectory("TestMachineModifierWithDivision2dVis");
           p_modifier->Setk_1(0.0);
           p_modifier->Setk_5(0.0);
           p_modifier->Setk_2(0.0);


           simulator.AddSimulationModifier(p_modifier);

       	unsigned num_machines=p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation());

       	TS_ASSERT_EQUALS(num_machines,1u);
           TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),1u);



           simulator.SetEndTime(2.5);
           simulator.Solve();
           unsigned num_machines3=p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation());
           TS_ASSERT_EQUALS(num_machines3,1u);
           TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),2u);
       }




	void xTestSingleCapsuleSimulationWithMachinesKiller()
            		   {
		EXIT_IF_PARALLEL;

		std::vector<Node<2>*> nodes;

		nodes.push_back(new Node<2>(0u, Create_c_vector(4.0, 4.0)));
		nodes.push_back(new Node<2>(1u, Create_c_vector(7.0, 4.0)));
		nodes.push_back(new Node<2>(2u, Create_c_vector(4.0, 5.0)));
		nodes.push_back(new Node<2>(3u, Create_c_vector(7.0, 5.0)));

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */
		NodesOnlyMesh<2> mesh;
		mesh.ConstructNodesWithoutMesh(nodes, 150.5);

		mesh.GetNode(0u)->AddNodeAttribute(0.0);
		mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.0 * M_PI;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(1u)->AddNodeAttribute(0.0);
		mesh.GetNode(1u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_THETA] = -0.0 * M_PI;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(1u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(2u)->AddNodeAttribute(0.0);
		mesh.GetNode(2u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_THETA] = -0.0 * M_PI;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(2u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		mesh.GetNode(3u)->AddNodeAttribute(0.0);
		mesh.GetNode(3u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_THETA] = 0.0 * M_PI;
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		mesh.GetNode(3u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		c_vector<double, 4> domain_size;
		domain_size[0] = -1000.0;
		domain_size[1] = 1000.0;
		domain_size[2] = -1000.0;
		domain_size[3] = 1000.0;
		mesh.SetInitialBoxCollection(domain_size, 10.0);

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */


		 // Create cells
		 std::vector<CellPtr> cells;
		 MAKE_PTR(WildTypeCellMutationState, p_state);
		 MAKE_PTR(DifferentiatedCellProliferativeType, p_type);
		 for (unsigned i=0; i<mesh.GetNumNodes(); i++)
		 {
			 UniformCellCycleModel* p_model = new UniformCellCycleModel();
			 p_model->SetMinCellCycleDuration(100.0);
			 p_model->SetMaxCellCycleDuration(1000.6);
			 CellPtr p_cell(new Cell(p_state, p_model));
			 p_cell->SetCellProliferativeType(p_type);



			 double rand_angle = M_PI/2.0;
			 std::vector<double> machine_angles;
			 machine_angles.push_back(rand_angle);
			 MAKE_PTR(TypeSixMachineProperty, p_property);
			 p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(1, machine_angles));
			 p_cell->AddCellProperty(p_property);

			 //double birth_time = -RandomNumberGenerator::Instance()->ranf();
			 p_cell->SetBirthTime(-0.9);
			 mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0;

			 cells.push_back(p_cell);

		 }

		 // Create cell population
		 NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

		 population.AddCellWriter<CellIdWriter>();
		 population.AddCellWriter<CapsuleOrientationWriter>();
		 population.AddCellWriter<CapsuleScalingWriter>();
		 population.AddCellWriter<MachineStateCountWriter>();

		 boost::shared_ptr<AbstractCentreBasedDivisionRule<2,2> > p_division_rule(new CapsuleBasedDivisionRule<2,2>());
		 population.SetCentreBasedDivisionRule(p_division_rule);

		 population.Update();

		 // Create simulation
		 OffLatticeSimulation<2> simulator(population);
		 simulator.SetOutputDirectory("TestSingleCapsuleWithMachinesKiller");
		 double dt = 1.0/1200.0;
		 simulator.SetDt(dt);
		 simulator.SetSamplingTimestepMultiple(10);

		 auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
		 simulator.SetNumericalMethod(p_numerical_method);


		 //
		 MAKE_PTR_ARGS(TypeSixMachineCellKiller<2>, p_killer, (&population));
		 simulator.AddCellKiller(p_killer);
		 /*
		  * We now create a capsuleforce law and pass it to the simulation
		  */
		 //auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
		 //p_capsule_force->SetYoungModulus(200.0);

		 //simulator.AddForce(p_capsule_force);
		 //
		 //auto p_boundary_condition = boost::make_shared<SquareBoundaryCondition>(&population);
		 //simulator.AddCellPopulationBoundaryCondition(p_boundary_condition);

		 MAKE_PTR(TypeSixMachineModifier<2>, p_modifier);
		 p_modifier->SetOutputDirectory("TestSingleCapsuleWithMachinesKiller");
		 p_modifier->Setk_1(0.0);
		 simulator.AddSimulationModifier(p_modifier);

		 /* We then set an end time and run the simulation */
		 simulator.SetEndTime(1.20527000050075); // was 1.0075
		 TS_ASSERT_EQUALS(p_modifier->GetTotalNumberOfMachines(simulator.rGetCellPopulation()),4u);

		 TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),4u);
		 simulator.Solve();
		 TS_ASSERT_EQUALS(simulator.rGetCellPopulation().GetNumRealCells(),2u);
            		   }


	void TestSingleCapsuleSimulationWithDivisionAndMachinesKiller()
            		   {
		EXIT_IF_PARALLEL;

		const unsigned num_nodes = 1u;
		//auto p_rand_gen = RandomNumberGenerator::Instance();

		// Create some capsules
		std::vector<Node<2>*> nodes;

		nodes.push_back(new Node<2>(0u, Create_c_vector(5.0, 5.0)));
		for (unsigned node_idx = 1; node_idx < num_nodes; ++node_idx)
		{
			c_vector<double, 2> safe_location;

			bool safe = false;
			while(!safe)
			{
				safe = true;
				safe_location = Create_c_vector(3.0, 3.0);

				for(auto&& p_node : nodes)
				{
					if(norm_2(p_node->rGetLocation() - safe_location) < 2.0)
					{
						safe = false;
					}
				}
			}

			nodes.push_back(new Node<2>(node_idx, safe_location));
		}

		/*
		 * We then convert this list of nodes to a `NodesOnlyMesh`,
		 * which doesn't do very much apart from keep track of the nodes.
		 */
		 NodesOnlyMesh<2> mesh;
		 mesh.ConstructNodesWithoutMesh(nodes, 100.0);
		 c_vector<double, 4> domain_size;
		 domain_size[0] = -1000.0;
		 domain_size[1] = 1000.0;
		 domain_size[2] = -1000.0;
		 domain_size[3] = 1000.0;
		 mesh.SetInitialBoxCollection(domain_size, 10.0);

		 mesh.GetNode(0u)->AddNodeAttribute(0.0);
		 mesh.GetNode(0u)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
		 mesh.GetNode(0u)->rGetNodeAttributes()[NA_THETA] = 0.0;
		 mesh.GetNode(0u)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
		 mesh.GetNode(0u)->rGetNodeAttributes()[NA_RADIUS] = 0.5;

		 for (unsigned node_idx = 1; node_idx < mesh.GetNumNodes(); ++node_idx)
		 {
			 mesh.GetNode(node_idx)->AddNodeAttribute(0.0);
			 mesh.GetNode(node_idx)->rGetNodeAttributes().resize(NA_VEC_LENGTH);
			 mesh.GetNode(node_idx)->rGetNodeAttributes()[NA_THETA] = 0.0;
			 mesh.GetNode(node_idx)->rGetNodeAttributes()[NA_LENGTH] = 2.0;
			 mesh.GetNode(node_idx)->rGetNodeAttributes()[NA_RADIUS] = 0.5;
		 }

		 // Create cells
		 std::vector<CellPtr> cells;
		 MAKE_PTR(WildTypeCellMutationState, p_state);
		 MAKE_PTR(TransitCellProliferativeType, p_type);
		 for (unsigned i=0; i<mesh.GetNumNodes(); i++)
		 {
			 UniformCellCycleModel* p_model = new UniformCellCycleModel();
			 p_model->SetMinCellCycleDuration(1.0);
			 p_model->SetMaxCellCycleDuration(1.6);
			 CellPtr p_cell(new Cell(p_state, p_model));
			 p_cell->SetCellProliferativeType(p_type);



			 double rand_angle = 2*M_PI*RandomNumberGenerator::Instance()->ranf()-M_PI;
			 std::vector<double> machine_angles;
			 machine_angles.push_back(rand_angle);
			 MAKE_PTR(TypeSixMachineProperty, p_property);
			 p_property->rGetMachineData().emplace_back(std::pair<unsigned, std::vector<double>>(1u, machine_angles));
			 p_cell->AddCellProperty(p_property);

			 //double birth_time = -RandomNumberGenerator::Instance()->ranf();
			 p_cell->SetBirthTime(-0.9);
			 mesh.GetNode(i)->rGetNodeAttributes()[NA_LENGTH] = 2.0 +3.0*p_cell->GetBirthTime()/p_model->GetCellCycleDuration(); ;

			 cells.push_back(p_cell);


		 }

		 // Create cell population
		 NodeBasedCellPopulationWithCapsules<2> population(mesh, cells);

		 population.AddCellWriter<CellIdWriter>();
		 population.AddCellWriter<CapsuleOrientationWriter>();
		 population.AddCellWriter<MachineStateCountWriter>();
		 population.AddCellWriter<CapsuleScalingWriter>();

		 boost::shared_ptr<AbstractCentreBasedDivisionRule<2,2> > p_division_rule(new CapsuleBasedDivisionRule<2,2>());
		 population.SetCentreBasedDivisionRule(p_division_rule);

		 // Create simulation
		 OffLatticeSimulation<2> simulator(population);
		 simulator.SetOutputDirectory("TestSingleCapsuleWithDivisionAndMachinesKiller");
		 double dt = 1.0/1200.0;
		 simulator.SetDt(dt);
		 simulator.SetSamplingTimestepMultiple(10);

		 auto p_numerical_method = boost::make_shared<ForwardEulerNumericalMethodForCapsules<2,2>>();
		 simulator.SetNumericalMethod(p_numerical_method);


		 //
		 MAKE_PTR_ARGS(TypeSixMachineCellKiller<2>, p_killer, (&population));
		 simulator.AddCellKiller(p_killer);
		 /*
		  * We now create a capsuleforce law and pass it to the simulation
		  */
		 auto p_capsule_force = boost::make_shared<CapsuleForce<2>>();
		 p_capsule_force->SetYoungModulus(200.0);

		 simulator.AddForce(p_capsule_force);
		 //

		 MAKE_PTR(TypeSixMachineModifier<2>, p_modifier);
		 p_modifier->SetOutputDirectory("TestSingleCapsuleWithDivisionAndMachinesKiller");
		 p_modifier->SetMachineParametersFromGercEtAl();
		 p_modifier->SetContactDependentFiring();
		 simulator.AddSimulationModifier(p_modifier);


		 /* We then set an end time and run the simulation */
		 simulator.SetEndTime(8.20527000050075); // was 1.0075
		 simulator.Solve();
		 PRINT_VARIABLE(simulator.rGetCellPopulation().GetNumRealCells());

     }



};

#endif /*TESTCAPSULESIMULATION2D_HPP_*/
