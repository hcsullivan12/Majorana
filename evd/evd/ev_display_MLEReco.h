#ifndef MLERECO_H
#define MLERECO_H

class MLEReco {

	public:
		MLEReco();
		void Start();

		double         diskRadius;
                double    incrementRadius;
                double     incrementTheta;
		int            incrementN;
                double  attenuationLength;
                double upperBoundN = 2500;
                int     numberOfPositions;
                double               beta;

	private:
		//std::string outputfile;
		//double data[numberOfPositions];
		//double data[numberOfPositions] = {1.16456, 0.4465, 0.231687, 0.147494, 0.122291, 0.125334, 0.172409, 0.483366};
		//double data[numberOfPositions] = {0.172409, 0.483366, 1.16456, 0.4465, 0.231687, 0.147494, 0.122291, 0.125334};
		//const double gain = 0.005; /// V/p.e.
		
		double ComputeLikelihood(double r, double theta, int N0);
		double ComputeLambda(double r, double theta, int N0, int m);
		void MakePlots(std::pair< double, std::vector<double> > parameters);		
	
};

#endif
