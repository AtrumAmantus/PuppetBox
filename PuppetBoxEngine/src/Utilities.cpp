#include <cctype>
#include <streambuf>
#include <utility>

#include <zip/zip.h>

#include "Logger.h"
#include "Utilities.h"

//OCUnusedGlobalDeclarationInspection - Utility Class - Might not be using all right now.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

// Using hardcoded values to save on processing.
#define PSEUDO_RANDOM 0.907,0.542,0.216,0.353,0.51,0.292,0.297,0.275,0.846,0.487,0.431,0.09,0.904,0.363,0.899,0.763,0.418,0.247,0.383,0.357,0.975,0.549,0.648,0.982,0.987,0.49,0.751,0.475,0.747,0.063,0.558,0.227,0.631,0.946,0.867,0.468,0.84,0.505,0.084,0.442,0.515,0.014,0.554,0.298,0.03,0.435,0.58,0.148,0.445,0.328,0.908,0.404,0.541,0.13,0.636,0.92,0.512,0.513,0.068,0.798,0.678,0.951,0.852,0.575,0.547,0.802,0.705,0.465,0.564,0.85,0.107,0.664,0.428,0.085,0.912,0.978,0.274,0.258,0.576,0.556,0.935,0.236,0.621,0.529,0.976,0.966,0.189,0.866,0.103,0.601,0.427,0.309,0.147,0.586,0.41,0.783,0.225,0.535,0.64,0.088,0.694,0.232,0.887,0.192,0.814,0.603,0.627,0.806,0.208,0.848,0.8,0.043,0.115,0.702,0.938,0.979,0.807,0.249,0.012,0.312,0.317,0.585,0.89,0.437,0.069,0.003,0.642,0.389,0.496,0.548,0.75,0.315,0.96,0.845,0.563,0.765,0.111,0.423,0.486,0.998,0.691,0.5,0.077,0.083,0.637,0.173,0.743,0.329,0.663,0.656,0.284,0.697,0.856,0.276,0.339,0.344,0.639,0.128,0.762,0.617,0.857,0.55,0.818,0.662,0.416,0.996,0.965,0.44,0.335,0.618,0.944,0.517,0.434,0.949,0.521,0.193,0.073,0.506,0.577,0.137,0.686,0.159,0.878,0.484,0.608,0.198,0.681,0.452,0.771,0.229,0.102,0.668,0.886,0.114,0.831,0.716,0.026,0.707,0.628,0.282,0.764,0.892,0.347,0.405,0.485,0.674,0.457,0.902,0.604,0.597,0.027,0.736,0.142,0.796,0.731,0.407,0.598,0.161,0.015,0.199,0.584,0.212,0.29,0.643,0.338,0.792,0.786,0.843,0.038,0.097,0.352,0.07,0.973,0.181,0.19,0.32,0.955,0.155,0.646,0.375,0.412,0.233,0.91,0.579,0.841,0.633,0.941,0.834,0.263,0.93,0.573,0.466,0.793,0.98,0.619,0.005,0.873,0.133,0.162,0.827,0.25,0.587,0.995,0.56,0.94,0.641,0.23,0.828,0.273,0.862,0.359,0.369,0.68,0.271,0.817,0.495,0.049,0.153,0.46,0.891,0.673,0.754,0.833,0.676,0.78,0.551,0.511,0.149,0.117,0.288,0.482,0.469,0.689,0.029,0.622,0.525,0.844,0.766,0.503,0.022,0.035,0.574,0.738,0.325,0.459,0.939,0.645,0.028,0.34,0.649,0.143,0.553,0.024,0.57,0.158,0.888,0.373,0.711,0.261,0.651,0.706,0.596,0.54,0.883,0.334,0.259,0.294,0.963,0.118,0.239,0.519,0.775,0.942,0.97,0.808,0.661,0.69,0.715,0.566,0.279,0.091,0.151,0.894,0.152,0.809,0.753,0.832,0.27,0.146,0.545,0.589,0.787,0.741,0.688,0.777,0.713,0.847,0.635,0.606,0.422,0.326,0.187,0.122,0.058,0.36,0.449,0.333,0.87,0.219,0.654,0.918,0.748,0.183,0.533,0.392,0.374,0.311,0.017,0.414,0.789,0.047,0.543,0.461,0.38,0.39,0.602,0.446,0.327,0.002,0.355,0.614,0.396,0.863,0.391,0.245,0.308,0.99,0.252,0.464,0.971,0.116,0.166,0.098,0.734,0.053,0.677,0.052,0.205,0.314,0.051,0.086,0.522,0.88,0.572,0.669,0.12,0.215,0.05,0.874,0.83,0.988,0.054,0.956,0.318,0.931,0.402,0.476,0.067,0.922,0.934,0.967,0.35,0.655,0.872,0.929,0.172,0.926,0.74,0.121,0.853,0.984,0.463,0.842,0.092,0.557,0.803,0.201,0.752,0.061,0.865,0.378,0.924,0.076,0.019,0.759,0.82,0.266,0.569,0.406,0.184,0.171,0.425,0.044,0.624,0.433,0.316,0.163,0.16,0.983,0.237,0.194,0.185,0.136,0.2,0.884,0.72,0.776,0.157,0.791,0.882,0.214,0.501,0.356,0.994,0.991,0.945,0.605,0.009,0.18,0.138,0.821,0.235,0.893,0.196,0.269,0.812,0.218,0.826,0.395,0.481,0.346,0.21,0.367,0.454,0.726,0.376,0.61,0.045,0.719,0.968,0.156,0.52,0.28,0.226,0.782,0.253,0.057,0.746,0.033,0.056,0.397,0.296,0.145,0.123,0.913,0.73,0.709,0.489,0.055,0.473,0.824,0.923,0.099,0.224,0.829,0.26,0.693,0.079,0.474,0.785,0.281,0.733,0.039,0.207,0.443,0.868,0.917,0.653,0.797,0.059,0.919,0.272,0.717,0.254,0.799,0.479,0.15,0.666,0.078,0.444,0.898,0.408,0.701,0.349,0.031,0.368,0.264,0.167,0.295,0.174,0.179,0.772,0.47,0.906,0.869,0.703,0.221,0.897,0.901,0.667,0.916,0.154,0.81,0.491,0.108,0.53,0.1,0.426,0.304,0.932,0.448,0.552,0.042,0.962,0.488,0.742,0.567,1,0.43,0.813,0.652,0.518,0.704,0.877,0.953,0.905,0.343,0.202,0.04,0.795,0.256,0.909,0.206,0.126,0.11,0.546,0.177,0.062,0.723,0.507,0.222,0.223,0.165,0.034,0.382,0.164,0.593,0.432,0.008,0.129,0.499,0.889,0.989,0.811,0.861,0.277,0.348,0.948,0.985,0.658,0.265,0.725,0.399,0.839,0.722,0.592,0.33,0.683,0.769,0.125,0.358,0.758,0.322,0.66,0.195,0.859,0.112,0.591,0.544,0.451,0.523,0.066,0.046,0.139,0.836,0.278,0.634,0.773,0.616,0.332,0.599,0.925,0.914,0.287,0.169,0.737,0.864,0.283,0.514,0.242,0.721,0.516,0.504,0.255,0.698,0.413,0.393,0.124,0.462,0.246,0.629,0.972,0.682,0.952,0.825,0.415,0.876,0.182,0.805,0.638,0.234,0.243,0.458,0.071,0.455,0.981,0.583,0.386,0.615,0.186,0.999,0.712,0.411,0.559,0.903,0.532,0.007,0.749,0.168,0.228,0.109,0.497,0.728,0.526,0.527,0.231,0.286,0.385,0.119,0.974,0.032,0.127,0.106,0.647,0.509,0.581,0.729,0.366,0.927,0.041,0.59,0.268,0.089,0.299,0.429,0.306,0.345,0.928,0.176,0.065,0.612,0.7,0.611,0.609,0.588,0.291,0.336,0.858,0.76,0.659,0.915,0.937,0.534,0.969,0.815,0.986,0.822,0.755,0.001,0.67,0.238,0.756,0.623,0.191,0.819,0.101,0.871,0.665,0.895,0.178,0.074,0.3,0.036,0.816,0.761,0.394,0.081,0.977,0.613,0.794,0.377,0.784,0.409,0.947,0.131,0.021,0.699,0.714,0.011,0.502,0.313,0.447,0.684,0.472,0.351,0.398,0.528,0.492,0.006,0.48,0.104,0.695,0.31,0.319,0.964,0.536,0.568,0.439,0.498,0.17,0.141,0.438,0.132,0.321,0.732,0.854,0.387,0.607,0.204,0.307,0.896,0.838,0.4,0.424,0.301,0.718,0.562,0.993,0.257,0.565,0.781,0.048,0.62,0.113,0.778,0.539,0.625,0.203,0.582,0.419,0.644,0.879,0.209,0.013,0.371,0.323,0.86,0.768,0.679,0.285,0.708,0.384,0.594,0.248,0.911,0.251,0.79,0.571,0.262,0.65,0.303,0.064,0.341,0.77,0.175,0.213,0.02,0.362,0.093,0.724,0.14,0.675,0.849,0.779,0.134,0.692,0.381,0.135,0.6,0.372,0.016,0.08,0.739,0.483,0.324,0.342,0.453,0.524,0.45,0.744,0.835,0.672,0.936,0.004,0.657,0.037,0.025,0.801,0.63,0.735,0.494,0.959,0.217,0.477,0.685,0.578,0.696,0.241,0.364,0.537,0.467,0.804,0.244,0.471,0.354,0.188,0.881,0.417,0.105,0.921,0.379,0.018,0.595,0.388,0.837,0.885,0.788,0.561,0.933,0.403,0.958,0.197,0.096,0.331,0.293,0.992,0.267,0.023,0.075,0.493,0.855,0.823,0.06,0.961,0.671,0.24,0.538,0.508,0.095,0.072,0.943,0.37,0.626,0.42,0.087,0.727,0.456,0.401,0.997,0.71,0.851,0.875,0.22,0.954,0.95,0.436,0.9,0.211,0.144,0.478,0.302,0.555,0.361,0.531,0.687,0.745,0.337,0.421,0.01,0.957,0.082,0.094,0.767,0.365,0.632,0.289,0.441,0.757,0.774,0.305
#define PSEUDO_COUNT 1000U;

#define PERLIN_NOISE 0.9490196078431372,0.9534313725490196,0.942156862745098,0.9196078431372549,0.9191176470588235,0.8823529411764706,0.9392156862745098,0.8813725490196078,0.9735294117647059,0.9215686274509804,0.9220588235294118,0.9009803921568628,0.9700980392156863,0.9892156862745098,0.8784313725490196,0.9852941176470589,0.9436274509803921,0.8926470588235295,0.9254901960784314,0.8897058823529411,0.9088235294117647,0.9446078431372549,0.878921568627451,0.9235294117647059,0.8931372549019608,0.9926470588235294,0.8852941176470588,0.8799019607843137,0.8862745098039215,0.9681372549019608,0.8779411764705882,0.9475490196078431,0.996078431372549,0.9338235294117647,0.9897058823529412,0.9117647058823529,0.875,0.8877450980392156,0.9715686274509804,0.9053921568627451,0.9210784313725491,0.9985294117647059,0.9823529411764705,0.9745098039215686,0.9323529411764706,0.8921568627450981,0.8803921568627451,0.8906862745098039,0.9029411764705882,0.961764705882353,0.8911764705882353,0.9181372549019607,0.9911764705882353,0.9480392156862745,0.9024509803921569,0.9176470588235294,0.9602941176470589,0.8848039215686274,0.9362745098039216,0.9416666666666667,0.9588235294117647,0.9573529411764706,0.9083333333333333,0.9607843137254902,0.9112745098039216,0.9558823529411765,0.9098039215686274,0.9406862745098039,0.9431372549019608,0.8985294117647059,0.888235294117647,0.9563725490196079,0.9127450980392157,0.9465686274509804,0.9524509803921568,0.9882352941176471,0.9156862745098039,0.9294117647058824,0.9872549019607844,0.9348039215686275,0.9044117647058824,0.9784313725490196,0.9401960784313725,0.9877450980392157,0.982843137254902,0.9264705882352942,0.9200980392156862,0.8950980392156863,0.9019607843137255,0.8975490196078432,0.9950980392156863,0.8946078431372549,0.9946078431372549,0.925,0.9450980392156862,0.9014705882352941,0.9068627450980392,0.8872549019607843,0.9058823529411765,0.9539215686274509,0.8754901960784314,0.9808823529411764,0.9142156862745098,0.9107843137254902,0.9774509803921568,0.9122549019607843,0.9397058823529412,0.9666666666666667,0.9769607843137255,0.9186274509803921,0.8838235294117647,0.957843137254902,0.9730392156862745,0.971078431372549,0.9411764705882353,0.9387254901960784,0.9318627450980392,0.967156862745098,0.9529411764705882,0.917156862745098,0.9553921568627451,0.9240196078431373,0.9284313725490196,0.9720588235294118,0.9598039215686275,0.9661764705882353,0.8764705882352941,0.9063725490196078,0.9004901960784314,0.9813725490196078,0.9857843137254902,0.9975490196078431,0.9357843137254902,0.9352941176470588,0.8774509803921569,0.9740196078431372,0.8936274509803922,0.9470588235294117,0.932843137254902,0.9367647058823529,1,0.9151960784313725,0.9166666666666666,0.9789215686274509,0.9764705882352941,0.9759803921568627,0.903921568627451,0.9862745098039216,0.8980392156862745,0.8828431372549019,0.9034313725490196,0.8833333333333333,0.9642156862745098,0.9676470588235294,0.8887254901960784,0.8955882352941177,0.9843137254901961,0.9647058823529412,0.9583333333333334,0.9794117647058823,0.9333333333333333,0.9965686274509804,0.9495098039215686,0.8759803921568627,0.8965686274509804,0.9504901960784313,0.9549019607843137,0.9093137254901961,0.9833333333333334,0.95,0.9245098039215687,0.9509803921568627,0.9568627450980393,0.8960784313725491,0.9593137254901961,0.8794117647058823,0.9382352941176471,0.8857843137254902,0.8941176470588236,0.9990196078431373,0.884313725490196,0.9230392156862746,0.9279411764705883,0.928921568627451,0.9137254901960784,0.9303921568627451,0.9848039215686275,0.9887254901960785,0.9622549019607843,0.9656862745098039,0.9299019607843138,0.9259803921568628,0.9818627450980392,0.9955882352941177,0.9225490196078432,0.986764705882353,0.9980392156862745,0.8916666666666667,0.9936274509803922,0.9696078431372549,0.9916666666666667,0.9779411764705882,0.9455882352941176,0.8808823529411764,0.9686274509803922,0.9627450980392157,0.9544117647058823,0.9931372549019608,0.9147058823529411,0.9,0.946078431372549,0.9901960784313726,0.9970588235294118,0.8818627450980392,0.9921568627450981,0.9274509803921569,0.8990196078431373,0.9691176470588235,0.9799019607843137,0.8901960784313725,0.9637254901960784,0.9725490196078431,0.9269607843137255,0.9519607843137254,0.9651960784313726,0.9161764705882353,0.975,0.9612745098039216,0.9313725490196079,0.9343137254901961,0.8995098039215687,0.8970588235294118,0.9372549019607843,0.8769607843137255,0.9485294117647058,0.9995098039215686,0.9426470588235294,0.990686274509804,0.9754901960784313,0.9205882352941176,0.9838235294117648,0.9308823529411765,0.907843137254902,0.8892156862745098,0.8867647058823529,0.9102941176470588,0.9941176470588236,0.9441176470588235,0.9377450980392157,0.9705882352941176,0.913235294117647,0.9073529411764706,0.9803921568627451,0.9049019607843137,0.9514705882352941,0.9632352941176471
#define PERLIN_NOISE_COUNT 256U;

namespace PB
{
	/**
	* \brief These hidden functions provide support for all whitespace characters, dependent on system definition of std::isspace().
	*/
	namespace
	{
		void ltrimWhitespace(std::string* str)
		{
			while (!str->empty() && std::isspace(str->at(0)))
			{
				str->erase(0, 1);
			}
		}
		std::string ltrimWhitespace(std::string str)
		{
			ltrimWhitespace(&str);

			return str;
		}
		void rtrimWhitespace(std::string* str)
		{
			while (!str->empty() && std::isspace(str->at(strlen((*str).c_str()) - 1)))
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		}
		std::string rtrimWhitespace(std::string str)
		{
			rtrimWhitespace(&str);

			return str;
		}
		void trimWhitespace(std::string* str)
		{
			ltrimWhitespace(str);
			rtrimWhitespace(str);
		}
		std::string trimWhitespace(std::string str)
		{
			trimWhitespace(&str);

			return str;
		}
		/**
		* \brief Finds the first occurance of any whitespace character in the given string.
		* 
		* \param str The string to search for whitespace.
		* 
		* \return The index of the first whitespace character found, returns std::string::npos if not found.
		*/
		std::size_t findWhitespace(std::string str)
		{
			std::size_t i;

			for (i = 0; i < str.length(); ++i)
			{
				if (std::isspace(str.at(i)))
				{
					return i;
				}
			}
			
			return std::string::npos;
		}
	}

	namespace StringUtils
	{
		void trim(std::string* str)
		{
			trimWhitespace(str);
		}

		std::string trim(std::string str)
		{
			return trimWhitespace(std::move(str));
		}

		void trim(std::string* str, const char& c)
		{
			ltrim(str, c);
			rtrim(str, c);
		}

		std::string trim(std::string str, const char& c)
		{
			trim(&str, c);

			return str;
		}

		void ltrim(std::string* str)
		{
			ltrimWhitespace(str);
		}

		std::string ltrim(std::string str)
		{
			return ltrimWhitespace(std::move(str));
		}

		void ltrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(0) == c)
			{
				str->erase(0, 1);
			}
		}

		std::string ltrim(std::string str, const char& c)
		{
			ltrim(&str, c);

			return str;
		}

		void rtrim(std::string* str)
		{
			rtrimWhitespace(str);
		}

		std::string rtrim(std::string str)
		{
			return rtrimWhitespace(std::move(str));
		}

		void rtrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(strlen((*str).c_str()) - 1) == c)
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		}

		std::string rtrim(std::string str, const char& c)
		{
			rtrim(&str, c);

			return str;
		}

		bool startsWith(const std::string& str, const std::string& prefix)
		{
			return str.substr(0, prefix.length()) == prefix;
		}

		void split(const std::string& str, std::string** splitValues, std::uint32_t* splitCount, bool excludeNull)
		{
			if (!str.empty())
			{
				*splitCount = 1;
				std::string currentSearchString = str;
				*splitValues = new std::string[*splitCount];
				std::size_t pos = findWhitespace(currentSearchString);

				while (pos != std::string::npos)
				{
					if (pos > 0 || !excludeNull)
					{
						++(*splitCount);

						// Copy previously found slices
						auto* tmpBits = new std::string[*splitCount];

						for (std::size_t i = 0; i < *splitCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = (*splitValues)[i];
						}

						delete[] *splitValues;
						*splitValues = tmpBits;

						// Add new slice
						(*splitValues)[*splitCount - 2] = currentSearchString.substr(0, pos);
						currentSearchString = currentSearchString.substr(pos + 1, currentSearchString.length() - pos);

						// Prep for next iteration
						pos = findWhitespace(currentSearchString);
					}
					else
					{
						// The currentSearchString started with the delimiter, just remove it and run again
						currentSearchString = currentSearchString.substr(1, currentSearchString.length() - 1);
					}
				}

				// Add the remaining string as the last entry in the array
				(*splitValues)[*splitCount - 1] = std::string{ currentSearchString };
			}
			else
			{
				*splitValues = nullptr;
				*splitCount = 0;
			}
		}

		void split(
		        const std::string& str,
		        const std::string& delimiter,
		        std::uint32_t splitLimit,
		        std::string** splitValues,
		        std::uint32_t* splitCount,
		        bool excludeNull)
		{
			if (!str.empty() && !delimiter.empty())
			{
				std::uint32_t maxNumberOfSplits = (splitLimit > 0) ? splitLimit : UINT32_MAX;
				*splitCount = 1;
				std::string currentSearchString = str;
				*splitValues = new std::string[*splitCount];
				std::size_t pos = currentSearchString.find(delimiter);

				while (maxNumberOfSplits > 0 && pos != std::string::npos)
				{
					--maxNumberOfSplits;

					if (pos > 0 || !excludeNull)
					{
						++(*splitCount);

						// Copy previously found slices
						auto* tmpBits = new std::string[*splitCount];

						for (std::size_t i = 0; i < *splitCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = (*splitValues)[i];
						}

						delete[] *splitValues;
						*splitValues = tmpBits;

						// Add new slice
						(*splitValues)[*splitCount - 2] = currentSearchString.substr(0, pos);
						currentSearchString = currentSearchString.substr(pos + delimiter.length(), currentSearchString.length() - pos);

						// Prep for next iteration
						pos = currentSearchString.find(delimiter);
					}
					else
					{
						// The currentSearchString started with the delimiter, just remove it and run again
						currentSearchString = currentSearchString.substr(1, currentSearchString.length() - 1);
					}
				}

				// Add the remaining string as the last entry in the array
				(*splitValues)[*splitCount - 1] = currentSearchString;
			}
			else if (!delimiter.empty())
			{
				*splitValues = new std::string[1]{ str };
				*splitCount = 1;
			}
			else
			{
				*splitValues = nullptr;
				*splitCount = 0;
			}
		}

		void split(
		        const std::string& str,
		        const std::string& delimiter,
		        std::string** splitValues,
		        std::uint32_t* splitCount,
		        bool excludeNull)
		{
			split(str, delimiter, 0, splitValues, splitCount, excludeNull);
		}
	}

	namespace RandomUtils
	{
		namespace
		{
			float perlinValues_[] = { PERLIN_NOISE };
			float pseudoRandom_[] = { PSEUDO_RANDOM };

			std::uint32_t nextPseudoIndex = 0;
		}

		float perlinValue(std::uint64_t offset)
		{
			offset %= PERLIN_NOISE_COUNT;
			return perlinValues_[offset];
		}

		float pseudoRand()
		{
			float value = pseudoRandom_[nextPseudoIndex];
			nextPseudoIndex++;
			nextPseudoIndex %= PSEUDO_COUNT;

			return value;
		}
	}

	namespace FileUtils
	{
		namespace
		{
			/**
			* \brief Custom buffer for building stream objects.
			*/
			struct membuf : std::streambuf {
				membuf(char const* base, std::size_t size) {
					char* p(const_cast<char*>(base));
					this->setg(p, p, p + size);
				}
			protected:
				pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
				{
					if (dir == std::ios_base::cur)
						gbump(static_cast<std::int32_t>(off));
					else if (dir == std::ios_base::end)
						setg(eback(), egptr() + off, egptr());
					else if (dir == std::ios_base::beg)
						setg(eback(), eback() + off, egptr());
					return gptr() - eback();
				}
			};
			/**
			* \brief Custom stream type for storing zip data.
			*/
			struct imemstream : virtual membuf, std::istream {
				imemstream(char const* base, std::size_t size)
					: membuf(base, size)
					, std::istream(static_cast<std::streambuf*>(this)) {
				}
			};
		}

		bool getFileListFromArchive(const std::string& archivePath, std::unordered_set<std::string>* files)
		{
			struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');

			if (zip != nullptr)
			{
				std::int32_t n = zip_entries_total(zip);

				if (n > -1)
				{
					if (files == nullptr)
					{
						files = new std::unordered_set<std::string>{};
					}

					for (std::int32_t i = 0; i < n; ++i)
					{
						zip_entry_openbyindex(zip, i);
						{
							files->insert(zip_entry_name(zip));
						}
					}

					return true;
				}
				else
				{
					LOGGER_ERROR("Error reading files from archive '" + archivePath + "'");
				}
			}
			else
			{
				LOGGER_ERROR("Error reading archive '" + archivePath + "'");
			}

			return false;
		}

		bool getContentsFromArchivedFile(
		        const std::string& archivePath,
		        const std::string& filePath,
		        std::int8_t** buffer,
		        std::size_t* bufferSize)
		{
			bool success = false;

			struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');
			{
				if (zip != nullptr)
				{
					zip_entry_open(zip, filePath.c_str());
					{
						if (zip != nullptr)
						{
							if (zip_entry_read(zip, (void**)buffer, bufferSize) >= 0)
							{
								success = true;
							}
						}
						else
						{
							LOGGER_ERROR("Error reading file '" + filePath + "' from archive '" + archivePath + "'");
						}
					}
				}
				else
				{
					LOGGER_ERROR("Error reading archive '" + archivePath + "'");
				}
			}

			zip_close(zip);

			return success;
		}

		bool getStreamFromArchivedFile(
		        const std::string& archivePath,
		        const std::string& filePath,
		        std::istream** stream)
		{
			bool success = true;
			std::int8_t* buffer = nullptr;
			std::size_t bufferSize = 0;

			if (getContentsFromArchivedFile(archivePath, filePath, &buffer, &bufferSize))
			{
				*stream = new imemstream((char*)buffer, bufferSize);
			}
			else
			{
				success = false;
				LOGGER_ERROR("Failed to read contents of '" + archivePath + "/" + filePath + "'");
			}

			return success;
		}
	}
}
#pragma clang diagnostic pop