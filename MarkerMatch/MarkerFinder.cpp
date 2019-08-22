#include "MarkerFinder.h"
#include <fstream>
#include <opencv2/imgproc/types_c.h>
#define  ENABLE_TEST  0

#define n48Size 901
//空心十字检测参数;
const double g_hcCoeff_48[n48Size] = {-0.043842,-0.018538,0.019566,0.021250,-0.046848,-0.020150,-0.017663,-0.035310,-0.025036,-0.007348,0.039808,0.130040,0.115198,0.007680,-0.037860,-0.033126,-0.048497,-0.045227,0.058688,0.086421,0.010981,0.034998,-0.019146,-0.034862,-0.031918,-0.048147,-0.028605,0.044151,0.068273,0.056848,0.094357,0.015445,-0.059533,-0.042164,-0.051508,-0.012849,0.021689,0.071478,0.083281,0.119746,-0.007059,-0.017744,-0.022719,-0.042239,0.001823,0.112803,0.111555,0.065052,0.031366,-0.036489,-0.012984,-0.032553,-0.030421,0.121696,-0.042131,-0.041505,-0.009199,0.098178,0.037784,0.001513,-0.026131,-0.030734,0.002789,0.051980,-0.011735,0.030995,0.049667,0.034641,0.009271,-0.032863,-0.038064,0.041086,0.124866,0.005947,-0.029734,-0.043204,-0.097938,-0.019030,-0.026110,0.023010,0.137147,0.060078,-0.015768,-0.032449,-0.031351,-0.057998,0.005901,0.020646,0.102090,0.113770,0.084541,0.004282,-0.012807,-0.011746,-0.064076,-0.015843,-0.041740,-0.027768,0.081651,0.001625,0.002606,-0.014690,0.001398,0.015733,0.039738,-0.006166,0.000810,0.043983,-0.002593,0.021350,0.023979,-0.002957,-0.029585,0.061322,0.054959,0.115056,0.068143,-0.041999,0.038760,0.029458,-0.013079,-0.022864,0.041129,0.037464,0.045444,0.019290,-0.030611,0.038478,0.040802,0.052401,0.021018,0.070056,0.027460,0.011947,-0.022734,-0.076350,0.032052,0.018551,0.010097,0.017697,0.050862,0.039575,0.022697,-0.015383,0.003285,0.020523,0.011998,-0.024474,-0.042355,-0.003212,0.022001,0.064176,0.060937,0.016687,-0.021826,-0.010303,0.012464,-0.002902,-0.044260,-0.068300,-0.026732,0.025837,-0.062722,0.009680,-0.012535,-0.045230,-0.021117,0.019770,0.021711,0.099370,0.081255,-0.056787,-0.032693,-0.041880,-0.021792,-0.001365,-0.035160,-0.018370,0.100650,0.018074,0.086322,0.104695,-0.003757,0.011052,0.016620,-0.003713,-0.013655,-0.040077,-0.056744,0.052169,0.030207,-0.061583,-0.016839,-0.018430,-0.010782,-0.023468,-0.025206,0.011644,0.033662,0.094273,-0.023115,0.017952,0.103527,0.039000,0.014708,0.018204,0.050482,0.042519,0.071705,-0.034802,-0.031287,-0.000003,0.008500,-0.024598,-0.015198,0.053198,0.043845,-0.015574,-0.009338,0.040524,0.020480,0.020079,0.011454,0.031229,0.009198,-0.051452,-0.033860,0.058399,0.092719,0.094774,0.022207,-0.017889,0.005796,-0.032316,0.086217,0.063334,0.041187,0.028668,-0.014801,-0.008583,-0.004530,-0.000288,-0.001985,0.055167,0.064684,0.046646,0.060241,0.026646,-0.002416,-0.013803,0.003563,-0.035582,-0.027829,0.006209,0.023397,0.070022,0.035111,0.021909,-0.018298,-0.007279,-0.049839,-0.011248,0.027976,0.005134,0.026726,0.079319,0.086172,0.038501,0.010101,-0.032299,-0.045740,-0.024850,-0.023775,0.037581,0.020891,0.020905,-0.009568,0.011771,-0.047736,-0.037127,-0.045791,-0.065831,-0.040511,0.002120,0.115853,0.046536,0.079579,-0.028229,-0.026926,0.006059,0.004670,0.038509,0.023711,0.030295,-0.015519,-0.006148,-0.006581,0.024658,-0.012795,-0.028721,0.002939,0.017948,0.006898,-0.026980,0.056141,0.083730,-0.009059,-0.030249,-0.046577,-0.026966,-0.020378,0.051946,-0.019745,0.097449,0.077544,0.134511,-0.016152,-0.051956,-0.034393,-0.059257,-0.043375,-0.050198,0.062731,0.096666,-0.031813,-0.024700,-0.040230,-0.034087,-0.017242,0.011642,0.009028,0.119392,0.086535,-0.090957,-0.029479,-0.060531,-0.039150,-0.045478,-0.018439,0.011500,0.171119,0.021586,0.086079,0.012931,0.015937,0.054975,0.028080,0.000075,-0.034782,0.062463,0.034283,-0.047910,0.021562,0.049323,0.062149,0.037860,0.042026,0.005979,0.118843,-0.033777,0.067053,0.051207,-0.044406,-0.010279,0.041452,0.050028,0.009748,0.017905,0.048863,0.068304,0.094987,0.006382,-0.015678,-0.025809,-0.009279,-0.039939,-0.035848,0.020824,-0.028719,-0.016313,-0.028404,0.014571,0.046274,0.053934,-0.040993,-0.034953,-0.025462,0.014464,-0.022379,-0.036587,-0.017927,-0.012930,-0.004143,-0.020127,0.068249,0.016381,0.100337,0.117605,0.038978,-0.004281,-0.062357,-0.021448,-0.025481,-0.048714,-0.021256,0.086870,0.107406,0.022582,0.004878,-0.044566,-0.018416,-0.029648,-0.050245,-0.069071,-0.013306,-0.047995,-0.055411,-0.039472,-0.045417,0.010357,0.050101,0.142433,0.082477,-0.095201,-0.044989,-0.046672,-0.060400,-0.047452,0.019385,0.034189,0.113291,0.005206,-0.026617,-0.009722,0.004908,0.026663,0.023392,0.019462,-0.004087,0.009849,-0.016423,-0.038073,-0.046513,-0.065310,-0.041206,0.012128,0.055227,0.036892,0.119959,0.068982,-0.076809,-0.011726,0.043803,-0.024712,0.014704,0.013814,0.009455,-0.014424,-0.038891,0.027134,0.132783,0.146731,0.118342,0.001162,-0.075439,-0.072050,-0.069492,-0.050220,-0.039048,-0.028407,-0.046383,-0.034334,-0.039273,-0.013473,-0.021812,0.112663,0.109880,0.087942,-0.002750,-0.018308,-0.014574,-0.024094,-0.026518,-0.040123,0.069207,0.117815,0.118216,0.159941,0.072821,0.023879,-0.055030,-0.104745,-0.087934,-0.070496,-0.086716,0.089910,0.121085,-0.025931,-0.049230,-0.063882,-0.043429,-0.058023,-0.074122,-0.016494,0.063172,0.010518,0.034104,0.073206,0.082794,-0.001567,-0.063910,-0.020126,0.010283,-0.042836,0.005773,0.057686,0.126474,0.150666,0.109375,-0.024737,-0.039114,-0.073005,0.053589,0.020915,-0.030432,0.000315,0.046966,-0.007193,-0.021898,-0.060522,-0.023560,-0.090036,-0.056491,-0.036741,0.052181,0.095849,0.098831,0.037156,0.005897,-0.086875,-0.002940,0.021983,0.005843,-0.014900,0.026277,0.040477,0.009731,0.016001,0.013886,0.068620,-0.010044,-0.038798,-0.042220,-0.048084,-0.031514,-0.002253,0.045470,0.117505,-0.044700,0.014653,0.007336,-0.046326,-0.081481,0.010188,0.097641,0.097677,0.072099,0.067788,0.014446,-0.021963,-0.042658,-0.082789,-0.028612,-0.006230,0.080525,0.070702,0.006735,-0.064189,-0.059690,-0.038316,-0.029098,0.005804,0.047824,0.141659,0.107765,-0.105363,-0.061596,-0.044462,-0.023803,-0.022158,0.058914,0.048164,0.136639,0.072431,0.042409,-0.027450,-0.023674,0.007874,0.029490,0.044869,-0.008834,0.070528,0.021581,-0.030815,-0.042772,-0.010588,0.057367,0.079166,0.068009,0.011377,0.014823,-0.006467,-0.043988,-0.011645,0.011453,-0.012799,-0.039188,0.034217,0.008904,0.009855,0.016956,0.040571,0.096905,0.170076,0.133805,0.003728,-0.084930,-0.095614,-0.081650,-0.026576,-0.040466,-0.066211,-0.013983,-0.002096,0.051988,0.089302,0.031557,0.028021,0.033241,-0.012618,0.017790,0.099230,0.113894,0.067982,-0.025641,-0.065703,-0.057365,-0.019386,0.086921,0.127313,0.062638,0.061196,-0.007637,-0.049227,-0.051676,-0.047424,-0.055061,0.111634,0.113256,-0.056190,-0.046380,-0.057483,-0.021592,-0.027235,-0.032898,0.012481,-0.010427,0.001049,0.055917,0.098606,0.073514,0.011742,-0.022559,-0.042530,-0.049914,0.046993,0.020461,-0.016127,0.037077,-0.002739,0.006141,-0.020042,-0.010962,0.057856,0.092395,0.077700,-0.043879,-0.030163,-0.022506,0.030239,0.010599,-0.014853,0.040516,-0.021870,0.052790,0.037111,0.048421,0.058897,0.046258,0.063884,0.045008,-0.060621,0.076998,0.087227,-0.038507,-0.033246,-0.068561,-0.022222,-0.011315,-0.006852,0.024598,0.058615,0.138051,0.009106,-0.027894,-0.060165,-0.028330,0.019854,0.016227,-0.117815,-0.016817,-0.004294,-0.009023,-0.024639,-0.063883,0.020946,0.068146,0.084480,0.085121,0.005513,-0.002405,-0.023672,-0.019733,0.001784,0.129225,-0.003639,0.059509,0.055065,-0.033392,-0.045819,-0.048622,-0.034910,-0.009696,0.043203,0.017669,-0.062805,-0.049264,-0.074319,-0.056552,-0.047829,-0.038692,0.038303,0.171467,0.085771,-0.031456,-0.060410,-0.000783,-0.012283,0.011758,0.025011,0.058897,0.080165,-0.037726,-0.034627,-0.034442,0.021304,0.002747,0.041761,0.028987,0.074133,0.071191,-0.021742,-0.032342,-0.043346,-0.048451,-0.005068,0.014284,0.004597,0.055750,0.137216,0.084892,0.032948,-0.013574,0.033772,0.041562,0.044712,0.025847,-0.001896,0.079965,0.028515,0.096964,0.078215,0.029478,0.001749,0.018224,-0.010492,-0.011208,0.027518,-0.018441,-0.022817,-0.000843,0.030076,-0.014485,-0.023901,0.019448,0.041143,0.021298,-0.040936,-0.076616,-0.020912,0.137317,0.043622,0.028716,-0.024802,-0.060005,-0.018003,-0.049672,-0.010972,0.095253,0.117705,0.103501,0.024450,-0.015079,-0.051849,0.014498,-0.023726,-0.039048,0.088019,-0.003957,-0.040546,0.031971,0.101099,0.061567,0.043617,0.016125,-0.043201,-0.050875,-0.013791,-0.026831,-0.002514,0.061996,0.010620,0.024200,0.031026,-0.020796,-0.000401,0.113769,0.069622,0.071226,0.083372,-0.021685,0.048853,0.061301,0.003134,0.017922,-0.005091,-0.016926,0.024167,0.062701,-0.058316,0.026423,0.077406,0.005922,-0.075787,0.086122,0.085913,0.026105,0.030218,-0.035276,-0.019836,0.002826,-0.016927,-0.008514,0.037121,0.158195,0.022785,-0.021129,-0.040803,-0.034517,-0.008816,-0.008658,-0.104444,0.018121,-0.006512,0.041848,0.065347,-0.042867,-0.020404,0.024170,-0.022423,-0.043444,-0.014970,-0.026824,-0.034510,-0.029537,-0.038353,-0.027590,-0.008349,0.021948,-0.009079,-2.746998};
//实心十字检测参数;
const double g_scCoeff_48[n48Size] = {0.011314,0.041145,0.057675,0.130656,0.088781,-0.079858,-0.072506,-0.041080,-0.046738,0.025202,0.000149,-0.025301,-0.019453,-0.085137,-0.106970,-0.057582,-0.030235,0.007541,0.007103,0.011266,0.006384,0.098947,0.082444,-0.058195,-0.059087,-0.019316,0.018208,0.082575,0.011456,-0.011543,0.038405,-0.085470,-0.085269,-0.050091,-0.030271,0.078290,-0.042340,-0.035960,0.017432,0.079057,-0.046214,-0.028014,-0.030239,-0.016573,-0.017372,-0.012556,0.054907,0.102632,0.188807,0.031923,-0.040985,-0.048416,-0.030870,-0.049767,0.019905,0.017895,0.002217,0.082681,-0.012965,-0.022146,-0.039347,-0.019410,0.008513,-0.031284,0.007253,0.035504,0.196752,0.095542,-0.057630,-0.017343,-0.032251,-0.070552,0.105658,0.098690,0.067543,0.109918,-0.098040,-0.048853,-0.021105,0.011177,0.043139,0.051382,0.060616,0.008331,-0.023571,-0.061553,0.164108,0.056854,0.044951,0.093435,-0.055186,0.015133,0.000760,0.147447,0.088393,-0.053860,-0.023555,-0.039840,-0.062287,-0.052095,0.003094,-0.023593,0.063499,0.041933,0.099484,-0.002882,-0.020020,-0.036303,-0.051628,-0.031506,-0.067733,-0.052857,0.062812,0.235697,0.078595,0.014363,-0.007800,-0.049636,-0.035965,-0.024505,0.018484,0.008096,0.106110,0.000176,0.007806,-0.029437,-0.117702,-0.060465,-0.110068,-0.030547,0.053531,0.173072,-0.011683,-0.011793,-0.022245,-0.013313,-0.038171,-0.063675,0.016391,-0.009211,0.055344,-0.038688,0.009852,0.012664,0.052642,-0.019582,-0.030243,-0.026103,-0.108094,-0.018586,-0.045125,0.008997,0.042899,-0.055782,-0.081878,-0.113922,0.017204,-0.025420,0.015135,-0.041481,0.019088,-0.047218,0.043802,0.012356,-0.056025,-0.028689,-0.096228,-0.030209,-0.057538,0.039653,0.087856,-0.061706,-0.030133,-0.068654,0.013588,-0.046902,0.071533,0.018619,0.096368,0.009966,0.044450,0.017646,0.012070,0.030004,0.026043,-0.055250,-0.050131,0.002908,0.034617,0.084468,-0.048613,0.015509,0.046547,-0.049915,-0.035531,-0.050400,-0.008366,0.077339,0.031029,0.046490,0.022449,0.101890,0.111128,-0.007779,-0.030595,-0.004941,-0.052328,0.055245,0.041432,0.046178,0.017803,-0.057158,-0.025395,-0.039695,0.001057,0.002620,0.103242,0.048883,0.016505,0.059221,-0.015432,-0.040746,-0.028975,0.000840,0.025973,0.031434,0.062761,0.015670,0.101946,0.054283,-0.053605,0.003101,-0.017391,-0.054647,0.072458,0.069463,0.019503,0.022086,-0.099341,-0.040739,-0.017738,0.022894,0.054627,0.038055,0.082265,0.032017,0.056166,-0.064366,-0.102193,-0.042648,-0.031961,0.008659,-0.070274,-0.001644,0.016088,0.135588,0.138237,0.040028,0.029980,-0.026429,-0.079364,-0.083545,-0.052794,-0.059138,0.078047,0.119422,0.134011,0.065743,0.009513,-0.017852,-0.033840,0.056532,0.049387,0.095374,0.022352,-0.025772,-0.019773,-0.041537,-0.100915,-0.181572,-0.103535,-0.099788,-0.039331,-0.051711,0.098598,0.077382,0.034799,-0.091145,-0.055693,-0.078202,-0.101765,-0.018436,0.082163,0.119329,0.004338,-0.010219,0.003332,0.054555,-0.022774,-0.060412,-0.006000,0.050059,0.063075,-0.006442,0.049230,0.105385,-0.060479,-0.101123,-0.136691,-0.086414,-0.055443,0.090796,0.028453,0.008728,-0.009052,0.088880,-0.014997,-0.060634,-0.043003,-0.035063,0.019302,-0.017688,0.028859,0.068801,0.052413,-0.013479,-0.055289,-0.041865,-0.058720,0.039663,-0.004015,0.073729,0.130994,-0.044835,-0.049972,-0.072006,-0.027274,-0.029464,0.108335,0.060991,0.073323,0.033894,0.061464,-0.007567,0.003518,-0.005582,-0.011904,0.040435,-0.059787,0.009203,0.055247,-0.023756,-0.059027,-0.011778,-0.027926,0.010287,0.102731,0.006140,0.048762,-0.016555,-0.050610,-0.037161,-0.032174,0.059666,0.159024,0.065864,0.034964,0.015314,-0.063173,0.095805,0.033756,-0.003882,0.030472,-0.062096,-0.042174,-0.050854,-0.016142,0.018140,-0.079300,-0.052260,-0.092418,-0.032884,0.040167,0.134180,0.069076,0.085905,0.034646,0.025773,-0.003054,-0.019341,-0.015868,-0.070494,-0.025843,-0.042265,0.046173,0.057486,0.103321,0.058056,-0.021250,0.005307,-0.119438,-0.045100,-0.016288,0.018936,0.126550,0.056904,0.073561,0.017747,-0.018481,-0.139257,-0.092248,-0.029884,-0.011082,0.029468,0.141461,0.052897,-0.020906,-0.027590,-0.069780,-0.026940,-0.037226,0.023920,0.093520,0.053490,-0.040158,-0.085819,-0.104926,-0.103472,0.015459,0.038334,0.074526,0.062430,-0.047330,0.043827,0.057143,0.052226,-0.089994,-0.035397,-0.012612,0.016116,0.007238,-0.071562,-0.047616,-0.021775,-0.022598,-0.082854,0.000779,0.057219,0.043477,-0.003371,-0.063407,-0.102996,-0.028898,0.000510,-0.029477,0.032158,0.103479,0.120826,0.039458,0.016087,0.085317,0.086201,0.131181,-0.001986,-0.062167,-0.011557,-0.003294,0.007830,0.030850,-0.048823,-0.065754,-0.029885,-0.113016,0.006440,-0.006796,0.005563,0.039552,0.168955,-0.009768,-0.052855,-0.000852,-0.056551,0.010834,-0.003565,0.070441,0.164160,0.120922,0.070770,0.037562,0.078159,-0.123440,-0.140317,-0.103445,-0.092943,0.007258,0.080988,0.011395,-0.076777,0.022546,-0.035317,-0.053463,-0.039132,-0.026387,0.126318,0.068243,-0.038906,-0.028777,0.027334,0.039678,-0.014356,-0.073756,-0.019260,0.097310,-0.028889,-0.043863,-0.012305,0.094843,0.143576,0.119369,-0.014083,0.009775,0.001580,0.076439,-0.032141,-0.085754,-0.003773,-0.042928,-0.067247,-0.067403,-0.035572,0.087313,0.059396,0.070081,0.041330,0.185174,0.075576,-0.055546,-0.094650,-0.050182,-0.022831,0.033576,-0.014416,-0.049663,-0.058342,-0.022962,0.074651,0.049014,0.111198,0.141773,0.108237,-0.012218,-0.045043,-0.062054,-0.053872,0.000870,0.001621,0.073967,0.119276,-0.040191,-0.048971,-0.059311,-0.048155,-0.047775,-0.013645,-0.010767,0.018396,-0.002081,-0.075930,-0.076808,-0.106607,-0.080176,-0.082534,-0.018192,-0.009615,-0.023730,-0.000458,0.059786,0.003190,-0.036214,-0.037003,0.032630,0.009696,-0.015911,0.017935,0.064133,-0.036004,-0.050594,-0.096230,-0.084881,0.047249,0.056943,0.061120,0.046850,0.027098,-0.085720,-0.060415,-0.067734,0.002715,0.050310,0.070854,0.024693,0.018515,-0.019691,-0.079369,-0.048973,-0.034463,0.040693,0.132943,0.121773,0.048109,0.001961,-0.028908,-0.071378,-0.090946,-0.071086,-0.000537,0.047173,0.047281,0.037003,0.037911,0.017214,-0.016783,-0.013305,0.032561,0.145203,0.085545,-0.065811,-0.089528,-0.079666,-0.028957,-0.033680,-0.043741,-0.030335,0.104342,0.145611,0.096378,0.023457,-0.012553,-0.008646,0.009611,-0.005878,-0.026409,0.065878,0.093207,0.113093,0.003727,-0.021256,0.023918,0.053303,-0.002744,0.023011,0.131815,0.050346,-0.064833,-0.079512,-0.079638,-0.036684,0.062233,0.011493,-0.000057,0.053080,0.005609,0.004425,-0.018753,-0.020624,0.079997,-0.013700,-0.029706,-0.009604,0.090969,0.031114,0.076745,-0.003245,-0.038770,-0.024424,-0.037444,0.031432,0.039634,0.098753,0.008367,0.091997,0.003694,-0.046439,0.004013,0.100107,-0.001315,-0.031796,0.016487,-0.078866,-0.018696,-0.016033,0.011498,0.085405,0.131068,0.092526,0.130904,0.117100,-0.061847,-0.053783,-0.048746,-0.020535,0.023317,-0.008174,0.012856,0.007219,0.067545,-0.094512,-0.010841,-0.030869,-0.049089,-0.017596,-0.090146,-0.020915,-0.030602,0.003473,-0.075948,-0.026321,-0.041912,-0.045370,-0.070439,-0.060400,-0.047181,-0.046365,-0.027407,0.039582,0.008280,0.021274,-0.005860,-0.027773,-0.059154,-0.032047,-0.067912,-0.065017,-0.033072,0.041278,0.029864,0.032107,0.004642,-0.057505,-0.069860,-0.048215,-0.056548,0.022161,-0.000420,0.000348,-0.001994,-0.008491,-0.042508,-0.021704,-0.017374,-0.094084,-0.073724,-0.000184,-0.027260,-0.037317,-0.004721,-0.127653,-0.058439,-0.032330,0.016534,0.070679,0.099368,0.068370,0.016858,-0.058790,-0.011510,-0.055876,0.007167,0.078766,0.163907,0.149232,0.064546,-0.000592,0.046019,-0.038163,-0.032316,-0.031174,-0.072286,-0.032153,0.032444,-0.037128,-0.058424,-0.041128,0.022210,-0.042770,-0.024302,-0.060940,0.051812,0.103058,0.066674,0.033556,0.039460,0.036262,-0.019778,-0.002395,0.045228,0.064831,0.085310,0.005558,-0.031724,0.020221,0.054600,0.063735,0.023632,0.077738,0.065457,0.082648,0.016696,-0.036066,-0.032122,0.136515,-0.013116,-0.040740,-0.059184,-0.044057,0.042540,0.028787,0.052706,0.128368,0.136611,0.096668,0.038638,0.071638,-0.074222,-0.037324,0.003004,0.029130,0.074784,0.062653,0.084864,0.054239,0.136682,0.074407,0.075828,0.026817,-0.049344,-0.098002,-0.050031,0.043709,0.016652,0.085217,0.013572,0.063780,-0.000996,-0.064357,-0.069863,0.037107,0.057436,0.046988,0.156617,-0.003964,-0.001629,-0.000784,0.008975,-0.053597,0.002663,0.003339,-0.027596,0.005979,-0.140064,-0.036107,-0.046271,-0.068453,-0.042326,0.015923,0.128287,0.099303,0.107498,-0.083149,-0.037610,-0.048452,-0.065666,-0.073769,-0.052328,-0.009728,-0.014688,0.006849,-0.034879,-0.053733,-0.060372,-0.035553,-0.109988,0.068545,0.029440,-0.013455,0.013498,-0.178925,-0.099996,-0.110373,-0.109605,0.009885,0.037769,-0.002000,-0.022974,0.036628,-0.095970,-0.110833,-0.108128,-0.071921,-0.021004,-2.031360};

CMarkerFinder::CMarkerFinder()
{
}


CMarkerFinder::~CMarkerFinder()
{

}

//y坐标的比较;
bool VecCmp_H(Vec<int, 5> a1, Vec<int, 5> a2) {
	return a1.val[1] < a2.val[1];
}

//x坐标的比较;
bool VecCmp_V(Vec<int, 5> a1, Vec<int, 5> a2) {
	return a1.val[0] < a2.val[0];
}

//nScale 缩放比例;霍夫变换找到的直线很多很接近，需要进一步处理，去掉一些多余的很接近的线;
void ReArrangeVector_H(vector<Vec<int, 5>> &vecLines, int nScale) {

	vector<Vec<int, 5>> newVector;
	int nSize = vecLines.size();

	//只有一条线的话，直接返回，不处理;
	if (nSize == 1)
		return;

	for (int i = 0; i < nSize - 1; i++)
	{
		Vec<int, 5> t0 = vecLines[i];
		Vec<int, 5> t1 = vecLines[i + 1];

		//找连续的靠的非常近的直线;
		int nStart = i;
		while (vecLines[i + 1].val[1] - vecLines[i].val[1] < 20 * nScale)
			if (i + 1 < nSize)
				i++;
		//一共有几条很近的直线
		int nGap = i - nStart;

		if (nGap == 0)
			newVector.push_back(t0);
		//两条很接近的线,取其均值;
		else if (nGap == 1)
		{
			Vec<int, 5> t;
			t.val[0] = t0.val[0];
			t.val[1] = (t0.val[1] + t1.val[1]) / 2;  //y坐标均值;
			t.val[2] = t0.val[2];
			t.val[3] = (t0.val[3] + t1.val[3]) / 2;  //y坐标均值;
			t.val[4] = t0.val[4];
			newVector.push_back(t);
		}
		//多条很接近的线;
		else
		{
			int nMiddle = nStart + (nGap + 1) / 2;
			newVector.push_back(vecLines[nMiddle]);
		}
	}

	//将最终结果写回;
	vecLines.clear();
	for (int i = 0; i < newVector.size(); i++)
		vecLines.push_back(newVector[i]);
}

//nScale 缩放比例;
void ReArrangeVector_V(vector<Vec<int, 5>> &vecLines, int nScale) {

	vector<Vec<int, 5>> newVector;
	int nSize = vecLines.size();

	//只有一条线的话，直接返回，不处理;
	if (nSize == 1)
		return;

	for (int i = 0; i < nSize - 1; i++)
	{
		Vec<int, 5> t0 = vecLines[i];
		Vec<int, 5> t1 = vecLines[i + 1];

		//找连续的靠的非常近的直线;
		int nStart = i;
		while (((vecLines[i + 1].val[0] - vecLines[i].val[0]) < 20 * nScale) && (i + 1 < nSize))
		{
			i++;
		}
		//一共有几条很近的直线
		int nGap = i - nStart;

		if (nGap == 0)
			newVector.push_back(t0);
		//两条很接近的线,取其均值;
		else if (nGap == 1)
		{
			Vec<int, 5> t;
			t.val[0] = (t0.val[0] + t1.val[0]) / 2;
			t.val[1] = t0.val[1];  //y坐标均值;
			t.val[2] = (t0.val[2] + t1.val[2]) / 2;
			t.val[3] = t0.val[3];  //y坐标均值;
			t.val[4] = t0.val[4];
			newVector.push_back(t);
		}
		//多条很接近的线;
		else
		{
			int nMiddle = nStart + (nGap + 1) / 2;
			newVector.push_back(vecLines[nMiddle]);
		}
	}

	//将最终结果写回;
	vecLines.clear();
	for (int i = 0; i < newVector.size(); i++)
		vecLines.push_back(newVector[i]);
}

//霍夫变换生成的坐标，很可能超出图像边界的。
//根据这些坐标，计算从上到下的直线，或者从左到右的直线的坐标;
void RecalPoint(Point &p1, Point &p2, int nHeight, int nWidth, int nAngle) {
	Point newP1, newP2;
	float f = (p1.y - p2.y) * 1.0 / (p1.x - p2.x);

	//垂直的线;
	if (nAngle < 45 || nAngle > 270 + 45) {
		newP1.y = 0;
		newP1.x = p1.x - (p1.y - newP1.y) / f;
		newP2.y = nHeight - 1;
		newP2.x = p1.x - (p1.y - newP2.y) / f;
	}
	//水平的线
	else
	{
		newP1.x = 0;
		newP1.y = p1.y - (p1.x - newP1.x) * f;
		newP2.x = nWidth - 1;
		newP2.y = p1.y - (p1.x - newP2.x) * f;
	}

	p1 = newP1;
	p2 = newP2;
}

//根据输入的图像,查找图像中的水平/垂直边缘;
bool CMarkerFinder::DetAlignment(Mat srcImage, int &nThre, vector<Vec<int, 5>> & vecFound) {
	if (srcImage.empty())
		return false;

	vecFound.clear();

	Mat resizedImg;  //缩小后处理;
	Mat BImg;        //二值图;
	int nScale = 4;

	resize(srcImage, resizedImg, cvSize(srcImage.cols / nScale, srcImage.rows / nScale));
	Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	GenerateBImg(resizedImg, BImg);
	dilate(BImg, BImg, kern);

	//对二值图进行膨胀,然后检测Marker区域;
	Mat erodeBImg;
	erode(BImg, erodeBImg, kern);
	subtract(BImg, erodeBImg, BImg);

	Mat tempImg; 
	tempImg = BImg;

	vector<Vec2f> lines;
	// 这里注意第五个参数，表示阈值，阈值越大，表明检测的越精准，速度越快，得到的直线越少（得到的直线都是很有把握的直线）
	//这里得到的lines是包含rho和theta的，而不包括直线上的点，所以下面需要根据得到的rho和theta来建立一条直线
	HoughLines(tempImg, lines, 1, CV_PI / 180, nThre);//注意第五个参数，为阈值
	while (lines.size() > 500){
		nThre = nThre + 100;
		HoughLines(tempImg, lines, 1, CV_PI / 180, nThre);
	}
	while (lines.size() > 50)
	{
		nThre = nThre + (lines.size() / 100 + 1) * 10; //如果数量过多,则每次加的阈值增量更大;
		lines.clear();
		tempImg = BImg;
		HoughLines(tempImg, lines, 1, CV_PI / 180, nThre);
	}
	while (lines.size() == 0)
	{
		nThre -= 10;
		lines.clear();
		tempImg = BImg;
		HoughLines(tempImg, lines, 1, CV_PI / 180, nThre);
	}

	vector<Vec<int, 5>> FinalLines_H, FinalLines_V;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0]; //就是圆的半径r
		float theta = lines[i][1]; //就是直线的角度
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b)) * nScale;
		pt1.y = cvRound(y0 + 1000 * (a))  * nScale;
		pt2.x = cvRound(x0 - 1000 * (-b))  * nScale;
		pt2.y = cvRound(y0 - 1000 * (a))  * nScale;
		int angle = int(theta / CV_PI * 180);

		//cout << "Line " << i << ": x1=" << pt1.x << ", x2=" << pt2.x << endl;

		//垂直的线;
		if (angle < 45 || angle > 270 + 45)
			FinalLines_V.push_back(Vec<int, 5>(pt1.x, pt1.y, pt2.x, pt2.y, angle));
		//水平的线;
		else
			FinalLines_H.push_back(Vec<int, 5>(pt1.x, pt1.y, pt2.x, pt2.y, angle));
	}
	//cout << endl;

	//排序;
	int nSize = FinalLines_H.size();
	std::sort(FinalLines_H.begin(), FinalLines_H.end(), VecCmp_H);
	nSize = FinalLines_V.size();
	std::sort(FinalLines_V.begin(), FinalLines_V.end(), VecCmp_V);

	//处理靠的太近的线段;
	vector<Vec<int, 5>> vecFinal;
	ReArrangeVector_H(FinalLines_H, nScale);
	ReArrangeVector_V(FinalLines_V, nScale);

	nSize = FinalLines_H.size();
	for (int i = 0; i < nSize; i++)
		vecFinal.push_back(FinalLines_H[i]);
	nSize = FinalLines_V.size();
	for (int i = 0; i < nSize; i++)
		vecFinal.push_back(FinalLines_V[i]);

	for (size_t i = 0; i < vecFinal.size(); i++)
	{
		Vec<int, 5> l = vecFinal[i];
		Point pt1, pt2;
		pt1.x = l[0];
		pt1.y = l[1];
		pt2.x = l[2];
		pt2.y = l[3];
		RecalPoint(pt1, pt2, srcImage.rows, srcImage.cols, l[4]);

		//最终图像画面内的直线的坐标;
		Vec<int, 5> newLine;
		newLine.val[0] = pt1.x;
		newLine.val[1] = pt1.y;
		newLine.val[2] = pt2.x;
		newLine.val[3] = pt2.y;
		newLine.val[4] = l[4];
		vecFound.push_back(newLine);
	}
}

//分别表示空心十字图，实心十字图,空心pattern图,实心pattern图;
bool CMarkerFinder::Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern, AlgParam param) {
	//hollow cross
	m_hcHog.blockSize = cvSize(16, 16);
	m_hcHog.blockStride = cvSize(8, 8);
	m_hcHog.cellSize = cvSize(8, 8);
	m_hcHog.winSize = cvSize(48, 48);
	m_hcHog.nlevels = 3;

	vector<float> x;
	for (int i = 0; i < n48Size; i++) {
		x.push_back(g_hcCoeff_48[i]);
	}
	/*
	ifstream fileIn;
	fileIn.open("d:\\clf.mod", ios::in);
	float val = 0.0f;
	while (!fileIn.eof())
	{
	fileIn >> val;
	x.push_back(val);
	}
	fileIn.close();

	*/
	if (x.size() == 0)
		return false;

	m_hcHog.setSVMDetector(x);

	//solid cross;
	m_scHog.blockSize = cvSize(16, 16);
	m_scHog.blockStride = cvSize(8, 8);
	m_scHog.cellSize = cvSize(8, 8);
	m_scHog.winSize = cvSize(48, 48);
	m_scHog.nlevels = 3;

	x.clear();
	for (int i = 0; i < n48Size; i++) {
		x.push_back(g_scCoeff_48[i]);
	}
	if (x.size() == 0)
		return false;

	m_scHog.setSVMDetector(x);

	m_hcMarker = hcMarker.clone();
	m_scMarker = scMarker.clone();
	m_hcPattern = hcPattern.clone();
	m_scPattern = scPattern.clone();
	if (m_hcMarker.empty() || m_scMarker.empty())
		return false;
	if (m_hcMarker.channels() == 3)
	{
		cvtColor(m_hcMarker, m_hcMarker, CV_BGR2GRAY);
	}
	if (m_scMarker.channels() == 3)
	{
		cvtColor(m_scMarker, m_scMarker, CV_BGR2GRAY);
	}
	//pattern图会被转化为二值图进行匹配;
	double dThre;
	if (!m_hcPattern.empty()) {
		if (m_hcPattern.channels() == 3)
			cvtColor(m_hcPattern, m_hcPattern, CV_BGR2GRAY);
		dThre = threshold(m_hcPattern, m_hcPattern, 0, 255, THRESH_OTSU);
	}
	if (!m_scPattern.empty()) {
		if (m_scPattern.channels() == 3)
			cvtColor(m_scPattern, m_scPattern, CV_BGR2GRAY);
		threshold(m_hcPattern, m_hcPattern, 0, 255, THRESH_OTSU);
	}

	m_algParams = param;

	return true;
}

//判断相机是否移动;nNum为判断出的移动的像素个数;
int CMarkerFinder::IsMoving(Mat preImg, Mat curImg) {
	Mat diff;
	absdiff(preImg, curImg, diff);

	Mat gray;
	if (diff.channels() == 3)
		cvtColor(diff, gray, CV_BGR2GRAY);
	else
		gray = diff;

	Mat  b;  //二值图;固定阈值;
	threshold(gray, b, 25, 255, THRESH_BINARY);  //粗调50和细调25，采用不同的参数;
	int nNonZero = countNonZero(b);

	return nNonZero;
}


//默认detector已经load了vector;
//dThre为svm检测时的阈值;
//bHollowCross：true表示检测的是空心十字, false表示检测是实心十字;
//vecFoud表示返回的检测结果;
bool CMarkerFinder::LocateCrossAreaByHog(Mat srcImg, double dHitThre, bool bHollowCross, vector<LocMarker> & vecFound) {

	if (bHollowCross && m_hcMarker.empty())
		return false;
	if (!bHollowCross && m_scMarker.empty())
		return false;
	if (srcImg.empty())
		return false;

	int nScale = 4;
	//实心十字缩放倍数小一点;
	if (!bHollowCross)
		nScale = 3;

	//缩放4倍后进行检测;
	Mat resizeSrcImg;
	resize(srcImg, resizeSrcImg, cvSize(srcImg.cols / nScale, srcImg.rows / nScale));
	Mat graySrcImg;
	if (resizeSrcImg.channels() == 3)
		cvtColor(resizeSrcImg, graySrcImg, CV_BGR2GRAY);
	else
		graySrcImg = resizeSrcImg;

	vector< Rect > detections;
	vector< double > foundWeights;
	if (bHollowCross)
		m_hcHog.detectMultiScale(graySrcImg, detections, foundWeights,dHitThre);
	else
		m_scHog.detectMultiScale(graySrcImg, detections, foundWeights, dHitThre);

	for (int i = 0; i < detections.size(); i++)
	{
		//实心标记检测时，权重过低的不要;经验值;
		/*if (!bHollowCross) {
			if (foundWeights[i] < 0)
				continue;
		}*/

		LocMarker lm;
		lm.rect = detections[i];
		lm.rect.x *= nScale;
		lm.rect.y *= nScale;
		lm.rect.width *= nScale;
		lm.rect.height *= nScale;
		lm.fConfidence = foundWeights[i]; 
		
		vecFound.push_back(lm);
	}

}

//利用梯度生成2值图;
void CMarkerFinder::GenerateBImg(Mat srcImg, Mat & bImg,GRADTYPE gt) {

	Mat gray;
	if (srcImg.channels() == 3)
		cvtColor(srcImg, gray, CV_BGR2GRAY);
	else
		gray = srcImg;

	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// 求 X方向梯度
	Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	/// 求 Y方向梯度
	Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	Mat input = grad;
	if (gt == GT_X)
		input = abs_grad_x;
	else if (gt == GT_Y)
		input = abs_grad_y;

	//二值化;
	double d = threshold(input, bImg, 0, 255, THRESH_OTSU);
}

//查找暗场下的marker
//srcImg & tempImg: 原图和模板图;
//wmp:定位晶片marker时的参数;
//vecLocRect: 返回的定位结果;
bool CMarkerFinder::FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp,
										vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect)
{
	int nSize = vecTextLoc.size();
	if (nSize == 0)
		return false;

	int nExtend = 10;
	for (int i = 0; i < nSize; i++)
	{
		Rect wafterRect;
		//默认maker在文字的左边;
		wafterRect.x = vecTextLoc[i].x - wmp.nDistToText - wmp.nMarkerWidth - nExtend;
		if (wafterRect.x < 0)
			wafterRect.x = 0;
		wafterRect.y = vecTextLoc[i].y;
		wafterRect.width = wmp.nMarkerWidth + nExtend * 2;
		
		int nTextBottom = vecTextLoc[i].y + vecTextLoc[i].height;
		//假设maker的底部坐标和文本坐标差不多。为了稳妥，增加了marker大小的高度;
		int nWafterBtm = nTextBottom + wmp.nMarkerWidth;
		if (nWafterBtm > srcImg.rows)
			nWafterBtm = srcImg.rows - 1;
		wafterRect.height = nWafterBtm - wafterRect.y;

		vecMarkerAreaRect.push_back(wafterRect);
	}

	//如果只有1个marker，还要找第二个;
	if (nSize == 1)
	{
		//缩放后的二值图;
		Mat resizedBImg;
		resize(bImg, resizedBImg, cvSize(bImg.cols / 8, bImg.rows / 8));
		
		//对二值图进行膨胀,然后检测Marker区域;
		Mat dilateBImg;
		Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		dilate(resizedBImg, dilateBImg, kern);

		//计算垂直投影：
		int h = dilateBImg.rows;
		int w = dilateBImg.cols;
		int * pHist = new int[w];
		memset(pHist, 0, sizeof(int)*w);
		for (int row = 0; row < h; row++)
		{
			for (int col = 0; col < w; col++)
			{
				//假设已有marker区域的左边还有一个marker
				//第二个marker的距离：5*wmp.nDistToText;
				int nMarkerLeft = vecMarkerAreaRect[0].x - 10*wmp.nDistToText - wmp.nMarkerWidth; 
				nMarkerLeft /= 8;
				int nMarkerRight = vecTextLoc[0].x + vecTextLoc[0].width;
				nMarkerRight /= 8;

				//已有marker区域不计算;
				if (col>=nMarkerLeft && col<=nMarkerRight)
					continue;

				char v1 = dilateBImg.at<char>(row, col);
				if (v1 != 0)
					pHist[col]++;
			}
		}

		//寻找candidate maker区域;
		vector<Rect> vecTemp;
		Rect rr;
		for (int k = 0; k < w;)
		{
			int nStart;
			if (pHist[k] > 0) {
				nStart = k;
				//找到第一个非黑点;
				while (pHist[k++]);
				//可能是marker;
				if ((k - nStart) * 8 > 0.7 * wmp.nMarkerWidth)
				{
					rr.x = nStart * 8;  //缩放8倍以后，需要算回原始坐标;
					rr.width = (k - nStart) * 8;
					rr.y = 0;
					rr.height = bImg.rows - 1;
					vecTemp.push_back(rr);

					//rectangle(srcImg, rr, Scalar(255, 255, 255), 8);
				}
			}
			else
				k++;
		}

		delete[] pHist;

		//只有一个区域的话，那就是这个marker;
		if (vecTemp.size() == 1)
		{
			rr = vecTemp[0];
			rr.x -= 30;
			rr.width += 60;
			vecMarkerAreaRect.push_back(rr);
		}
		//多个区域的话，不管是2个还是3个，都是第二个;
		else if (vecTemp.size() > 1)
		{
			rr = vecTemp[1];
			rr.x -= 20;
			rr.width += 40;
			vecMarkerAreaRect.push_back(rr);
		}
	}

	return true;
}

//定位明场/暗场下的pattern.为了加快速度,可以缩小数倍后再匹配;
bool CMarkerFinder::LocatePattern(Mat srcImg, bool bHcPattern,int nMaxCount, vector<LocMarker> & vecFound)
{
	Mat patternImg;
	if (bHcPattern)
		patternImg = m_hcPattern;
	else
		patternImg = m_scPattern;

	if (patternImg.empty())
		return false;

	//imwrite("d:\\pattern.jpg", patternImg);

	Mat resizedTempImg, resizedSrcImg;
	int nScale = 4;
	resize(srcImg, resizedSrcImg, cvSize(srcImg.cols / nScale, srcImg.rows / nScale));
	resize(patternImg, resizedTempImg, cvSize(patternImg.cols / nScale, patternImg.rows / nScale));

	//最多两个,匹配阈值0.5;
	vector<LocMarker> vecRect;
	LocateTemplate(resizedSrcImg, resizedTempImg, 0.5, nMaxCount, vecRect);
	for (int j = 0; j < vecRect.size(); j++)
	{
		vecRect[j].rect.x *= nScale;
		vecRect[j].rect.y *= nScale;
		vecRect[j].rect.width *= nScale;
		vecRect[j].rect.height *= nScale;
		vecFound.push_back(vecRect[j]);
	}

	return true;
}

//通过最后一位数字，验证模板匹配是否合理。
//有时板和片上的编号只差一位数字，非常接近，这时定位容易出错;
//srcImg & tempImg: 原图和模板图;
//fMatchThre:匹配度阈值;
//r: 原图中定位到的匹配区域;
bool CMarkerFinder::CheckLocTemp_ByLastNum(Mat srcImg, Mat tempImg, float fMatchThre, Rect r) {
	float fRatio = m_algParams.locpattern_fRatio;

	//模板中最后一位数字的子区域
	Mat temp2;
	Rect rp;
	if (m_algParams.locpattern_bVerticalNum)
		rp = Rect(0, tempImg.rows * fRatio, tempImg.cols, tempImg.rows*(1 - fRatio));
	else
		rp = Rect(tempImg.cols*fRatio, 0, tempImg.cols*(1 - fRatio), tempImg.rows);
	temp2 = tempImg(rp);

	float fRatioSrc = fRatio - 0.1; //保证比模板的区域稍微大一点;
	Mat src1 = srcImg(r);
	Rect rs(0, src1.rows*fRatioSrc, src1.cols, src1.rows * (1-fRatioSrc));
	Mat src2 = src1(rs);

	Mat resImg;
	matchTemplate(src2, temp2, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

	double minValue, maxValue;
	Point minLoc, maxLoc;
	minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
	cout << "2nd max_value= " << maxValue << endl;
	if (maxValue < fMatchThre)
		return false;
	else
		return true;
}


//在图中查找模板图片;
//srcImg & tempImg: 原图和模板图;
//nMaxCount: 图中最多可能存在的模板;
//vecTempRect:返回值，表示找到的模板区域;
bool CMarkerFinder::LocateTemplate(Mat srcImg, Mat tempImg, float fMatchThre, int nMaxCount, vector<LocMarker> & vecTempRect) {
	
	Mat srcGrayImg;
	Mat tempGrayImg;

	if (srcImg.channels() == 3)
		cvtColor(srcImg, srcGrayImg, CV_BGR2GRAY);
	else
		srcGrayImg = srcImg;
	if (tempImg.channels() == 3)
		cvtColor(tempImg, tempGrayImg, CV_BGR2GRAY);
	else
		tempGrayImg = tempImg;

	//对二值化图像进行匹配;
	Mat srcBImg;
	Mat resImg;
	double dthre = threshold(srcGrayImg, srcBImg, 0, 255, THRESH_OTSU);
	cout << "Threshold = " << dthre << endl;

	matchTemplate(srcBImg, tempGrayImg, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

	double minValue, maxValue;
	Point minLoc, maxLoc;
	int nSpace = 25;

	for (int i = 0; i < nMaxCount; i++)
	{
		minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
		if (maxValue < fMatchThre)
			continue;
		cout << "max_value= " << maxValue << endl;
		cout << "maxLoc_x=" << maxLoc.x << ",maxLoc_y=" << maxLoc.y << endl;

		int startX = maxLoc.x - nSpace;
		if (startX < 0) startX = 0;
		int startY = maxLoc.y - nSpace;
		if (startY < 0) startY = 0;
		int endX = maxLoc.x + nSpace;
		if (endX > resImg.cols - 1)
			endX = resImg.cols - 1;
		int endY = maxLoc.y + nSpace;
		if (endY > resImg.rows - 1)
			endY = resImg.rows - 1;

		//将最高匹配点周围的数据都清空，以免下次匹配还是在附近：
		Mat temp = Mat::zeros(endX - startX, endY - startY, CV_32FC1);
		temp.copyTo(resImg(Rect(startX, startY, temp.cols, temp.rows)));

		LocMarker lm;
		Rect k;
		k.x = maxLoc.x;
		k.y = maxLoc.y;
		k.width = tempImg.cols;
		k.height = tempImg.rows;

		bool bRet = true;
		if (m_algParams.locpattern_bCheckLastNum)
			bRet = CheckLocTemp_ByLastNum(srcBImg, tempGrayImg, fMatchThre, k); //最后一位数字要再次确认;

		if (!bRet)
			continue;
		else
		{
			lm.fConfidence = maxValue;
			lm.rect = k;
			vecTempRect.push_back(lm);
		}

	}

	return vecTempRect.size();
}

//精确查找Marker;
//srcImg:原图;
//bHollowCross: true为空心十字模板图,false为实心十字模板图;
//dMatchThre:匹配度阈值;
//vecMarkerAreaRect: maker的大致区域，非精细;
//vecMarkerLoc:精确的Marker定位结果,输出参数;
bool CMarkerFinder::LocateMarkerByTempMatch(Mat srcImg, bool bHollowCross, vector<Rect> vecMarkerAreaRect,
	                                  float dMatchThre,vector<LocMarker> &vecMarkerLoc)
{
	int nSize = vecMarkerAreaRect.size();
	if (nSize == 0)
		return false;

	Mat grayTempImg;
	if (bHollowCross)
		grayTempImg = m_hcMarker;
	else
		grayTempImg = m_scMarker;
	
	for (int i = 0; i < nSize; i++) {
		Rect r = vecMarkerAreaRect[i];

		//模板的大小必须比区域更小才能匹配;
		if ((grayTempImg.rows > r.height) || (grayTempImg.cols > r.width))
			continue;

		Mat roiImg = srcImg(r);

		Mat graySrcImg;
		Mat BSrcImg;

		if (roiImg.channels() == 3)
			cvtColor(roiImg, graySrcImg, CV_BGR2GRAY);
		else
			graySrcImg = roiImg;

		threshold(graySrcImg, BSrcImg, 0, 255, THRESH_OTSU);

		//二值图像的匹配：
		Mat resImg;
		matchTemplate(BSrcImg, grayTempImg, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

		double minValue, maxValue;
		Point minLoc, maxLoc;
		minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);

		//匹配度过小的不要;
		if (maxValue < dMatchThre)
			continue;

		//切换回全图坐标;
		Rect k;
		LocMarker lm;
		k.x = maxLoc.x + r.x;
		k.y = maxLoc.y + r.y;
		k.width = grayTempImg.cols;
		k.height = grayTempImg.rows;

		lm.rect = k;
		lm.fConfidence = maxValue;
		vecMarkerLoc.push_back(lm);
	}

	return true;
}

//查找mask的实心十字marker区域;
bool CMarkerFinder::FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect) {
	
	int nDilateKernalSize = 5;

	//对每一组文字区域进行处理; 
	for (int i = 0; i < vecTextLoc.size();	i++)
	{
		Rect curR = vecTextLoc[i];
		Mat roiImg = bImg(curR);

		//将二值图横纵坐标都缩小8倍
		Mat resizedBImg;
		resize(roiImg, resizedBImg, cvSize(roiImg.cols / 8, roiImg.rows / 8));

		//对二值图进行膨胀,然后检测文字区域;
		Mat dilateBImg;
		Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(nDilateKernalSize, nDilateKernalSize));
		dilate(resizedBImg, dilateBImg, kern);

		//保存膨胀后的图;
		if (ENABLE_TEST) {
			char strFile[256];
			sprintf_s(strFile, "d:\\dialateBImg_%d.jpg", i);
			imwrite(string(strFile), dilateBImg);
		}

		//计算膨胀后的二值图的水平投影;
		int h = dilateBImg.rows;
		int w = dilateBImg.cols;
		int * pHist = new int[h];
		memset(pHist, 0, sizeof(int)*h);
		for (int row = 0; row < h - 1; row++)
		{
			for (int col = 0; col < w; col++)
			{
				char v1 = dilateBImg.at<char>(row, col);
				if (v1 != 0)
					pHist[row]++;
			}
		}

		//查找区域;
		int nStart = 0;
		int nContinusBlack = 0;

		Rect t; //marker区域;
		for (int j = 0; j < h;)
		{
			if (pHist[j] > 0) {
				
				nStart = j;
				while (pHist[j++] && j<h);  //查找白色区域;
				 
				//只选择有一定长度的区域;
				int nLen = j - nStart+1;  //白色区域的长度;
				if (nLen > 22 && nLen < 42 )  //白色区域长度的限制;
				{
					t.y = nStart + nDilateKernalSize;  //原始大小需要加上膨胀时的核大小;
					t.height = nLen + 1 - nDilateKernalSize *2;
					t.x = 0;
					t.width = w;

					//回到原图坐标;
					t.x *= 8;
					t.y *= 8;
					t.width *= 8;
					t.height *= 8;
					//加上ROI区域坐标;
					t.x += curR.x;
					t.y += curR.y;
					vecMarkerRect.push_back(t);
				}

				nContinusBlack = 0;  //连续黑色区域的长度重置;
			}
			else
			{
				j++;
				nContinusBlack++;
			}
		}

		delete[] pHist;
	}
	
	return true;
}

//bImg:输入的二值图;
//struLTParam:用于文字定位的参数;
//nScale:原图的缩放比例;
//查找文字区域的左右垂直边界
void CMarkerFinder::FindTextCord(Mat bImg, LocTexParam struLTParam,vector<Rect> & vecRect) {

	int h = bImg.rows;
	int w = bImg.cols;

	//垂直投影，找阶跃点;
	int * nHist = new int[w];
	memset(nHist, 0, sizeof(int)*w);
	for (int r = 0; r < h - 1; r++)
	{
		//uchar* pdata = img.ptr<uchar>(r);
		for (int c = 0; c < w; c++)
		{
			char v1 = bImg.at<char>(r, c);
			char v2 = bImg.at<char>(r + 1, c);
			if (v1 != v2)
				nHist[c]++;
		}
	}

	//查找条状文字区域,定位左右坐标;
	int nExtend = 20;  //拓展20个像素;
	int nStepThre = struLTParam.nStepCountThre;
	int nTextWidThre = struLTParam.nTextBandWThre;
	for (int i = 0; i < w - 1;)
	{
		//查找具有一定阶跃点的坐标;
		if (nHist[i] > nStepThre) {
			int nStart = i;
			while (nHist[i++] > nStepThre);
			if ((i - nStart) > nTextWidThre)
			{
				Rect rr; 
				//左右边界;
				int x1 = (nStart - nExtend) > 0 ? (nStart - nExtend) : 0;
				int x2 = (i + nExtend) < w ? (i + nExtend) : (w - 1);

				//高度默认为全图高度,上下边界稍后定位;
				rr.x = x1;
				rr.width = x2 - x1 + 1;
				rr.y = 0;
				rr.height = (h - 1);
				vecRect.push_back(rr);
			}
		}
		else
			i++;
	}

	delete[] nHist;

	//定位每个区域的上下边界;
	for (int i = 0; i < vecRect.size(); i++)
	{
		Mat roiBImg = bImg(vecRect[i]);

		int nRoiH = roiBImg.rows;
		int nRoiW = roiBImg.cols;

		int * pHHist = new int[h];
		memset(pHHist, 0, sizeof(int)*h);
		for (int r = 0; r < nRoiH - 1; r++)
		{
			for (int c = 0; c < nRoiW; c++)
			{
				char v1 = roiBImg.at<char>(r, c);
				if (v1)
					pHHist[r]++;
			}
		}

		//分别从上到下记录第一个非零的位置;
		int nTop = 0;
		int nBottom=h-1;
		while (pHHist[nTop++] == 0);
		while (pHHist[nBottom--] == 0);

		//稍微拓展下;
		nTop = (nTop - nExtend) > 0 ? (nTop - nExtend) : 0;
		nBottom = ((nBottom + nExtend) < h) ? (nBottom + nExtend) : (h - 1);
		vecRect[i].y = nTop;
		vecRect[i].height = nBottom - nTop;

		delete[] pHHist;
	}
}

//定位文字区域;
//struLTParam: 文字定位时用的参数;
//bImg:返回的二值图;
//vecLoc:返回的文字区域区间，仅限横坐标;
bool CMarkerFinder::LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc) {
	int nScale = 2; 

	//缩小一半进行处理;
	Mat resizedSrcImg;
	resize(srcImg, resizedSrcImg, cvSize(srcImg.cols / nScale, srcImg.rows / nScale));
	struLTParam.nTextBandWThre /= nScale;

	Mat gray;
	cvtColor(resizedSrcImg, gray, CV_BGR2GRAY);

	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// 求 X方向梯度
	Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	/// 求 Y方向梯度
	Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	//二值化;
	double d = threshold(grad, bImg, 0, 255, THRESH_OTSU);
	
	if (ENABLE_TEST)
	{
		imwrite("d:\\grad.jpg", grad);
		imwrite("d:\\bimg.jpg", bImg);
	}

	//查找文字区域;
	FindTextCord(bImg, struLTParam, vecLoc);
	//坐标切回到原图大小;
	for (int i = 0; i < vecLoc.size(); i++)
	{
		vecLoc[i].x *= nScale;
		vecLoc[i].y *= nScale;
		vecLoc[i].width *= nScale;
		vecLoc[i].height *= nScale;
	}

	//画区域;
	//for (int i = 0; i < vecLoc.size(); i++)
	//	rectangle(srcImg, vecLoc[i], Scalar(0, 0, 255), 8);

	if (ENABLE_TEST)
		imwrite("d:\\res.jpg", srcImg);

	//找不到文字区;
	if (vecLoc.size() == 0)
		return false;

	//改回原来的大小
	resize(bImg, bImg, cvSize(bImg.cols * nScale, bImg.rows * nScale));

	return true;
}

void CMarkerFinder::FinalFinetune(Mat srcImg, Mat &bImg,Rect &rectH,Rect &rectS) {

	Mat b;
	Mat GrayImg;
	cvtColor(srcImg, GrayImg, CV_BGR2GRAY);
	double dthre = threshold(GrayImg, b, 0, 255, THRESH_OTSU);
	float fRatio = countNonZero(b)*1.0 / (b.rows*b.cols*1.0);
	cout << "1: non-zero=" << fRatio << endl;

	//白色占比达到一定比例，保证二值化后黑色十字比较精准;
	while (fRatio < 0.6) {
		dthre = threshold(GrayImg, b, dthre - 10, 255, THRESH_BINARY);
		fRatio = countNonZero(b)*1.0 / (b.rows*b.cols*1.0);
		dthre -= 10;
	}
	
	//实心十字检测;
	Mat resImg;
	matchTemplate(b, m_scMarker, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

	double minValue, maxValue;
	Point minLoc, maxLoc;

	Rect r_sc;
	minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
	r_sc.x = maxLoc.x;
	r_sc.y = maxLoc.y;
	r_sc.width = m_scMarker.cols;
	r_sc.height = m_scMarker.rows;
	rectS = r_sc;
	/*
	Mat beforeB = b;
	rectangle(beforeB, r_sc, Scalar(0, 0, 0), 1);
	namedWindow("before_bimg", 0);
	resizeWindow("before_bimg", 684, 456);
	imshow("before_bimg", beforeB);
	*/
	//虚心十字检测：
	b(r_sc).setTo(Scalar(255, 255, 255));  //实心区域先全部变成白色;

	Mat tempV;
	Mat newTemp;  //实际的空心十字,可能和模板大小稍有差异;
	Rect r;
	r.x = m_algParams.finetune_nHcMargin;
	r.y = m_algParams.finetune_nHcMargin;
	r.width = m_hcMarker.cols - 2*m_algParams.finetune_nHcMargin;
	r.height = m_hcMarker.rows - 2*m_algParams.finetune_nHcMargin;
	newTemp = m_hcMarker(r);
	bitwise_not(newTemp, tempV);  //空心十字，反色;

	//保证虚心十字区域二值化后有一定数量;
	int nSize = b.rows*b.cols;
	int nZeroCount;
	nZeroCount = nSize - countNonZero(b);
	while (nZeroCount < 0.05 * nSize)
	{
		threshold(GrayImg, b, dthre + 10, 255, THRESH_BINARY);
		b(r_sc).setTo(Scalar(255, 255, 255));  //实心区域先全部变成白色;
		nZeroCount = nSize - countNonZero(b);
		dthre += 10;

		if (dthre > 230)
			break;
	}

	matchTemplate(b, tempV, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

	Rect r_hc;
	minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
	r_hc.x = maxLoc.x;
	r_hc.y = maxLoc.y;
	r_hc.width = tempV.cols;
	r_hc.height = tempV.rows;
	rectH = r_hc;

	rectangle(b, r_sc, Scalar(0, 0, 0), 1);
	rectangle(b, r_hc, Scalar(0, 0, 0), 1);
	
	bImg = b;
}

void CMarkerFinder::Test() {
	imwrite("d:\\hollowcross.jpg", m_hcMarker);
	imwrite("d:\\hcpattern.jpg", m_hcPattern);
}