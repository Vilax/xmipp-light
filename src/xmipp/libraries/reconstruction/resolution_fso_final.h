/***************************************************************************
 *
 * Authors:    Jose Luis Vilas (joseluis.vilas-prieto@yale.edu)
 *                             or (jlvilas@cnb.csic.es)
 *              Hemant. D. Tagare (hemant.tagare@yale.edu)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/

#ifndef _PROG_RES_DIR
#define _PROG_RES_DIR

#include <core/xmipp_program.h>
#include <core/xmipp_fftw.h>
#include <core/metadata_extension.h>
#include <random>


class ProgFSOFinal : public XmippProgram
{
public:
        // Filenames
        FileName fnhalf1, fnhalf2, fnmask, fn_3dfsc, fn_ani, fnOut;

        // Double Params
        double sampling, ang_con, thrs;

		// Int params
		int Nthreads;
		size_t xvoldim, yvoldim, zvoldim, fscshellNum;

		//long params
		//number of Fourier elements
		long Ncomps;
        
        // Bool params
        bool test, do_3dfsc_filter;

        //Matrix2d for the projection angles
        Matrix2D<float> angles;

		//Frequency vectors and frequency map
		Matrix1D<double> freq_fourier_x;
		Matrix1D<double> freq_fourier_y;
		Matrix1D<double> freq_fourier_z;
		MultidimArray<float> fx, fy, fz;
	    MultidimArray<float> threeD_FSC, normalizationMap, aniFilter;
		MultidimArray< double > freqMap;

		//Half maps
		MultidimArray< std::complex< double > > FT1, FT2;
		MultidimArray<float> real_z1z2, absz1_vec, absz2_vec;

		//Access indices
		MultidimArray<long> freqElems, cumpos, freqidx, arr2indx;


public:
        // Defining the params and help of the algorithm
        void defineParams();

        // It reads the input parameters
        void readParams();

        // The output is a multidim array that define INVERSE of the value of the frequency in Fourier Space
        // To do that, it makes use of myfftV to detemine the size of the output map (myfftV and
        // the output will have the same size), and the vectors freq_fourier_x, freq_fourier_y, 
        // and freq_fourier_z that defines the frequencies along the 3 axis. The output will be
        // sqrt(freq_fourier_x^2+freq_fourier_y^2+freq_fourier_x^2)
        void defineFrequencies(const MultidimArray< std::complex<double> > &myfftV,
    		                                    const MultidimArray<double> &inputVol);

        // Estimates the global FSC between two half maps FT1 and FT2 (in Fourier Space)
        void fscGlobal(double sampling_rate,
                 MultidimArray< double >& freq,
                 MultidimArray< double >& frc,
    			 double maxFreq, int m1sizeX, int m1sizeY, int m1sizeZ, MetaData &mdRes,
				 double &fscFreq, double &thrs, double &resInterp);

        // Defines a map (sphere) in Fourier space after ffsshift (complete Fourier space) for which
        // each radius defines the frequency of that point in Fourier Space
        void createfrequencySphere(MultidimArray<double> &sphere,
    		Matrix1D<double> &freq_fourier_x,
			 Matrix1D<double> &freq_fourier_y,
			 Matrix1D<double> &freq_fourier_z);

       
        // Defines a Matrix2D with coordinates Rot and tilt achieving a uniform coverage of the
        // projection sphere. Bool alot = True, implies a dense converage
        void generateDirections(Matrix2D<float> &angles, bool alot);

        void anistropyParameter(const MultidimArray<double> FSC,
    		MultidimArray<double> &directionAnisotropy, size_t dirnumber,
			MultidimArray<double> &aniParam, double thrs);
    
        void anistropyParameterSimple(const MultidimArray<double> FSC,
			MultidimArray<double> &aniParam, double thrs);
        
        void prepareData(MultidimArray<double> &half1, MultidimArray<double> &half2, bool test);

        void saveFSCToMetadata(MetaData &mdRes,
    		const MultidimArray<double> &freq,
			const MultidimArray<double> &FSC, FileName &fnmd);

        void saveAnisotropyToMetadata(MetaData &mdAnisotropy,
    		const MultidimArray<double> &freq,
			const MultidimArray<double> &anisotropy);

        void directionalFilter(MultidimArray<std::complex<double>> &FThalf1,
    		MultidimArray<double> &threeDfsc, MultidimArray<double> &filteredMap, 
            int m1sizeX, int m1sizeY, int m1sizeZ);

		void directionalFilter_reading(MultidimArray<std::complex<double>> &FThalf1,
    		MultidimArray<double> &threeDfsc, MultidimArray<double> &filteredMap, 
            int m1sizeX, int m1sizeY, int m1sizeZ);
        
        void resolutionDistribution(MultidimArray<double> &resDirFSC, FileName &fn);

        void getCompleteFourier(MultidimArray<double> &V, MultidimArray<double> &newV,
    		int m1sizeX, int m1sizeY, int m1sizeZ);

        void createFullFourier(MultidimArray<double> &fourierHalf, FileName &fnMap,
    		int m1sizeX, int m1sizeY, int m1sizeZ);

        void run();

		void arrangeFSC_and_fscGlobal(double sampling_rate, 
						double &thrs, double &resInterp, MultidimArray<double> &freq);

        // Estimates the directional FSC between two half maps FT1 and FT2 (in Fourier Space)
        // requires the sampling rate, and the frequency vectors, 
		void fscDir_fast(MultidimArray<double> &fsc, double rot, double tilt,
				         MetaData &mdRes, MultidimArray<double> &threeD_FSC, 
						 MultidimArray<double> &normalizationMap,
						 double &fscFreq, double &thrs, double &resol, size_t dirnumber);
};

#endif