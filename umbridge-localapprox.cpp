#include <MUQ/Modeling/UMBridge/UMBridgeModPiece.h>
#include <MUQ/SamplingAlgorithms/ExpensiveSamplingProblem.h>
#include <MUQ/SamplingAlgorithms/SingleChainMCMC.h>
#include <MUQ/SamplingAlgorithms/MCMCFactory.h>

namespace pt = boost::property_tree;

using namespace muq::Utilities;
using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;



class ExampleModel : public umbridge::Model {
public:

  ExampleModel(std::string host)
   : umbridge::Model("posterior") // Give a name to the model
  {
    // Set up a simple MCMC method
    pt::ptree pt;
    pt.put("NumSamples", 1e2); // number of MCMC steps
    pt.put("BurnIn", 0);
    pt.put("PrintLevel",3);
    pt.put("KernelList", "Kernel1"); // Name of block that defines the transition kernel
    pt.put("Kernel1.Method","MHKernel");  // Name of the transition kernel class
    pt.put("Kernel1.Proposal", "MyProposal"); // Name of block defining the proposal distribution
    pt.put("Kernel1.MyProposal.Method", "AMProposal"); // Name of proposal class
    pt.put("Kernel1.MyProposal.InitialVariance", 10);
    pt.put("Kernel1.MyProposal.AdaptSteps", 100);
    pt.put("Kernel1.MyProposal.AdaptStart", 100);
    pt.put("Kernel1.MyProposal.AdaptEnd", 1000);

    pt.put("RegressionOptions.NumNeighbors", 10);
    pt.put("NumNeighbors", 10);

    //pt.put("GammaScale", 1e-4);
    //pt.put("TailCorrection", 1);
    //pt.put("LyapunovScale", 1e-2);
    pt.put("GammaScale", .1);
    pt.put("TailCorrection", 0);
    pt.put("LyapunovScale", 1.0);

    posterior = std::make_shared<UMBridgeModPiece>(host, "posterior");//, config);

    samplingProblem = std::make_shared<ExpensiveSamplingProblem>(posterior, pt);
  }

  std::vector<std::size_t> GetInputSizes(const json& config_json) const override {
    return {2};
  }

  std::vector<std::size_t> GetOutputSizes(const json& config_json) const override {
    return {1};
  }

  std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>>& inputs, json config) override {

    std::vector<Eigen::VectorXd> inputs_eigen = UMBridgeModPiece::StdVectorsToEigenvectords(inputs);

    auto samplingState = std::make_shared<SamplingState>(inputs_eigen);
    samplingState->meta["iteration"] = cntSamples;
    samplingState->meta["IsProposal"] = false;
    double logdensity = samplingProblem->LogDensity(samplingState);
    cntSamples++;

    std::cout << "logdensity " << logdensity << " for" << std::endl << samplingState->state[0]  << std::endl;
    std::cout << "Evaluations: " << cntSamples << " LA vs. " << samplingProblem->cumbeta + samplingProblem->cumgamma + samplingProblem->cumkappa << " model" << std::endl;


    return {{logdensity}};
  }

  // Specify that our model supports evaluation. Jacobian support etc. may be indicated similarly.
  bool SupportsEvaluate() override {
    return true;
  }

private:
  unsigned int cntSamples = 1;
  std::shared_ptr<UMBridgeModPiece> posterior;
  std::shared_ptr<ExpensiveSamplingProblem> samplingProblem;
};


int main(int argc, char* argv[])
{
  if (argc <= 1) {
    std::cerr << "Call with: ./binary HOSTNAME:PORT" << std::endl;
    exit(-1);
  }
  std::string host(argv[1]);

  ExampleModel model(host);
  umbridge::serveModels({&model}, "0.0.0.0", 4241);

}