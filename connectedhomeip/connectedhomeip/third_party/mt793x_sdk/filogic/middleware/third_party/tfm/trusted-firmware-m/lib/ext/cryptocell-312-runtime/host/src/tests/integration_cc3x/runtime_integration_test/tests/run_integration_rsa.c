/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <limits.h>

/* mbedtls lib */
#include "mbedtls/rsa.h"
#include "mbedtls/timing.h"
#include "mbedtls/bignum.h"
#include "mbedtls/sha1.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

/* cc api */
#include "cc_rnd_common.h"

/* CC pal */
#include "cc_pal_types.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

#if defined(MBEDTLS_RSA_C)

/************************************************************
 *
 * defines
 *
 ************************************************************/
#define RUNIT_RSA_4096 0

/************************************************************
 *
 * typedefs
 *
 ************************************************************/
typedef struct RunItRsaData_t
{
    int mod;
    const char *name;
    const char *input_P;
    const char *input_Q;
    const char *input_N;
    const char *input_E;
    const char *input_D;
    const char *private_result_hex_str;
    const char *public_result_hex_str;
} RunItRsaData_t;

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static const RunItRsaData_t data[] =
{
    {
        2048,
        "2048",
        "e79a373182bfaa722eb035f772ad2a9464bd842de59432c18bbab3a7dfeae318c9b915ee487861ab665a40bd6cda560152578e8579016c929df99fea05b4d64efca1d543850bc8164b40d71ed7f3fa4105df0fb9b9ad2a18ce182c8a4f4f975bea9aa0b9a1438a27a28e97ac8330ef37383414d1bd64607d6979ac050424fd17",
        "c6749cbb0db8c5a177672d4728a8b22392b2fc4d3b8361d5c0d5055a1b4e46d821f757c24eef2a51c561941b93b3ace7340074c058c9bb48e7e7414f42c41da4cccb5c2ba91deb30c586b7fb18af12a52995592ad139d3be429add6547e044becedaf31fa3b39421e24ee034fbf367d11f6b8f88ee483d163b431e1654ad3e89",
        "b38ac65c8141f7f5c96e14470e851936a67bf94cc6821a39ac12c05f7c0b06d9e6ddba2224703b02e25f31452f9c4a8417b62675fdc6df46b94813bc7b9769a892c482b830bfe0ad42e46668ace68903617faf6681f4babf1cc8e4b0420d3c7f61dc45434c6b54e2c3ee0fc07908509d79c9826e673bf8363255adb0add2401039a7bcd1b4ecf0fbe6ec8369d2da486eec59559dd1d54c9b24190965eafbdab203b35255765261cd0909acf93c3b8b8428cbb448de4715d1b813d0c94829c229543d391ce0adab5351f97a3810c1f73d7b1458b97daed4209c50e16d064d2d5bfda8c23893d755222793146d0a78c3d64f35549141486c3b0961a7b4c1a2034f",
        "3",
        "77B1D99300D6A54E864962DA09AE10CF19A7FB888456BC2672B72AEA52B204914493D16C184AD201EC3F762E1FBD8702BA796EF953D9EA2F26300D285264F11B0C8301D0207FEB1E2C984445C899B0ACEBAA74EF014DD1D4BDDB43202C08D2FF9692D8D788478DEC829EB52AFB5AE068FBDBAC499A27FACECC391E75C936D55F07BB45EE184DAB45808E15722502F279F89B38C1CB292557E5063597F52C75D61001EDC33F4739353E33E56AD273B067C1A2760208529EA421774A5FFFCB3423B1E0051E7702A55D80CBF2141569F18F87BFF538A1DA8EDBB2693A539F68E0D62D77743F89EACF3B1723BDB25CE2F333FA63CACF0E67DF1A431893BB9B352FCB",
        "48ce62658d82be10737bd5d3579aed15bc82617e6758ba862eeb12d049d7bacaf2f62fce8bf6e980763d1951f7f0eae3a493df9890d249314b39d00d6ef791de0daebf2c50f46e54aeb63a89113defe85de6dbe77642aae9f2eceb420f3a47a56355396e728917f17876bb829fabcaeef8bf7ef6de2ff9e84e6108ea2e52bbb62b7b288efa0a3835175b8b08fac56f7396eceb1c692d419ecb79d80aef5bc08a75d89de9f2b2d411d881c0e3ffad24c311a19029d210d3d3534f1b626f982ea322b4d1cfba476860ef20d4f672f38c371084b5301b429b747ea051a619e4430e0dac33c12f9ee41ca4d81a4f6da3e495aa8524574bdc60d290dd1f7a62e90a67",
        "1f5e927c13ff231090b0f18c8c3526428ed0f4a7561457ee5afe4d22d5d9220c34ef5b9a34d0c07f7248a1f3d57f95d10f7936b3063e40660b3a7ca3e73608b013f85a6e778ac7c60d576e9d9c0c5a79ad84ceea74e4722eb3553bdb0c2d7783dac050520cb27ca73478b509873cb0dcbd1d51dd8fccb96c29ad314f36d67cc57835d92d94defa0399feb095fd41b9f0b2be10f6041079ed4290040449f8a79aba50b0a1f8cf83c9fb8772b0686ec1b29cb1814bb06f9c024857db54d395a8da9a2c6f9f53b94bec612a0cb306a3eaa9fc80992e85d9d232e37a50cabe48c9343f039601ff7d95d60025e582aec475d031888310e8ec3833b394a5cf0599101e"
    },
    {
        3072,
        "3072",
        "f5b86d4a3aa83ad24b334c7457e6b1ded044093e69fa19ebef2e24df9aca6c55d2a6cb84683667ff9cfd8f4537234c9ba4e5de9ca6833f4fa08a0a0d5516e2606095528c48dfae424f3df0009a749b0ad67cf879a523c3a0fc5c46428be1a7f368165959fbe3b33c122c8d2681260cfb8bc4814404778662ded71c08beec6ada120141efde7fbb92a1ca2e0cffae7ec5cb43a6db42218af06ae14e0eba5b9289aef170837f2bbeb144cedd5cfb27e46d8f2784baeb1c772273e70752db31363b",
        "c10d3ca1b2711eb97f0a567f476cad607fcbf552cd449f7f0919287ce77ab0c2371034df58133416dc4ef21d490a82d0dbf0897b363bb01e3a7f5d223020e5d9368baa685cb17f395625bab672722abafc20985e95f952282868894085bdfcde13a63f71db21cfbb20d13168c80939602add3a56f653ecb97fcfad4955959da35d8e3621f89639953d4c569c146c00c3e452523d931fe75424b39c4a1bcd6c572d7d334f30b7fc7fa736ab1579381eeafc2c46ca8cc3330fe55a3514193b85a7",
        "b94cbef4f0dec0355c88bb3e6ab31dfb8c9c1176fa16d95519c6f8a663b8116235435c02a5a9b76591c380cae0ea7f9e381052a7d34dddc4f234f45db969d7900fce21d5bbc6bb300289ce595ffd1b0d85d749a717a326fee838f0eab063f1db8b7a21e187f41a9e826ea67c2f870c568c563b086c1c17c9221b7e59b268a989023d39508efa07e40d99aa003378307c535156ad5235265b89bae1c6182b8beceb0504855fdfa0c80e967f49484d21a2656d848a4f88224cdb5589cbb871a7f165914e37c34e1a9957cee1366b1ff8ec7900ec559c0cf4fffc5b3f473b509aa3a9084858adaa848262fd8158ce8bedd5a0a9f09631f993cf7b25178186d67e85ac7c42d1b206ef02d8f8ec75575253e4fff2caf74740d929316d3a6982c38eeb4c815188fd822697462aa327daaf0b021f3b86c3c8d4d927e13737991ea4623efc95c04f90ddf5bb5554a16fc64d540beb64599ad4a8d4e7ae58ac452eeaec5697f61d5951bbf82f7c1e9a8a7f1a7af23e3c3ef4c81ea0c591d4992cd0e0077d",
        "10001",
        "2DC97AFE076EE34BCC3BB518ED40A94DF926AF6218D60089F8BEA3BE69B1980A0E63409F1ED45B0EEBFFC5195C8E554E8147F744A4FFFEA6A5BAD40F3CBB9C9A6A6D630AEF456207B0F8E0565153B79595D6E6CA351B098DBF5022A7BB4257070AE5F8ED43ACDBCF0327D857A35ED97BA15DC564856243B65D8931A574624C52208D8456159E4EC8D95B7918679F714BA742D80A27E01E444C3A3383C18131282FC2A3A5595566E3484783D855D749525981C05874E28CA7352363D34516408B074426AADE995682C4D278F2046DE0235B947D0C246499706EFB86B19B5BED7FCCB446F58C9A7F93FAC0D3DEC6EA96D51F1A24062385440D2384F402AF87CA3FEE9F091A5FF3D1085984AE24193B8E78C1B8E78DB87035DCCC03E9B65B8AA6540EB42D79FFC298A3FB59027B78A87BAD2AF7A5ADC58BA8FD7B1BB944308AE066E5340231CE704F4420D9A43EACD5C6910FBA253ABA238FCA02D6185B106B8B03A67D09586D318390DB8676641E72890F1F9834BDF2B43C3590AB4927531BE239",
        "9f351ced000825b86f53020dce5712f0865b023ffc981b26c5c308ca64aaabbf50b66198cc95e6411a602d62af45daec586184308c99cf05af66451ffb76c025d5305539cf2b600d42cac6b1669955b2436738d87c8b78e6d57c3217752fff86c269a14645b62e110f5f04f4675ca06d2575276da5a6d884052a19decd4a002300a201184cb256c02e895a32c5b0e59c9c0ecf7af9aa64aeaa48f83f9b729ff2ab3f21846c42ec9748fa18e49d297c59f7acc0deb3718b4c820a6cbb090b946f4d32f7ed47ec0f2e8b883c718a651ab3916173af7b0a8ae718617c9b6cb4ac4e9c47dbebc71f7737551d23d6a69eca1aeb9d54214e65fa992f4871d99674dc95ab374c42414dea78284e3c9ff5e9384c873a5f4f9440a6eb8423f80aeeed9a96c57f2db523f16f8a2b82c092d92bf0fb2140d26e2c4f18f91bcf2d1fd8db593d53ae67f5b8efad2a0543cf5aca71b738ca100d2ff52ee5483e4e423b5b1064d58375635ffad4d195cdbe7755c246304dcbe897fba1e318b7b12b28866de496e4",
        "a18ea22a5c6b4d09b5ec2a3fbce5e63a871a831375e8a26c8befdb98bde625d66c44e4938127d1bfdd624000eca308e218d518d2b26df415ab44252d81d6f92602a7e00437e55399e8f434b055c35321b33df6e6bf15bcbea456f5cda48f3de9f429f7254140fc006ade528ae302c077f69f783dfa1269905d3808976322386d6479e0d6811bf629e33f21e9e54aae85fbf521360bae30c00c6eadd2b1771809e0ecc693d6f9ce94b826b7e71fa59a005e7824d085ab0e21831c1676afaa83150cc52a315b422ad28f5bb9a4efeb0d18c48d0ae5ca76b3ad75e51138005db530c2b253089f3d8e7e7c5c39cc59746db8c284e9cebb672a47e3802873912c44d6e307b449bac83e17f97a5be9c0b4d9830a734a4f7b88995c6e674fdff93e7d6bf7fd79d6c5ddbe28a787520e6d8027c0c771c2dcd487444091da5478758db52ea66ef35c11e944601fea643732a55578a1367426d4268c728355a2c5000aad23e341669e08abf7516cd67a7fe48127a9c823ac6c1a75f722cbca51a45c0d8516"
    },
    {
        4096,
        "4096",
        "d8d78135a5456957e4970b3b6bea0350ba05ec296734a211c4aa7177ecb7f78e58f40213d68c2184ed11688bdc0e3acc3f97b3fec287ca9d79714d39014c5e5d5cbdd540d88cfa72b4a06b404e083b6c08c554fa91dff7c690e72999da594ab2ed373e92f0353e398adfa2de2d6ab72f3dd82d7bdcdff698ba2e2ba6800ba6cdab2124a3b06b3699eec1a0c636bfdc4ae42c598738b0cc394a5c17c2ced0ee0b0cf9ca82e38a46f8448003523305254b1fecbba2efd94811cf2de6d0b9ff9d0bf8f2b468ad92f01384e746b3d41ddd9842629e9e344405573c026d1325652bac954679e6216aea8b52c7e19018d23317b8242cc3acbb31efe408b3ef437f1b87",
        "c78c3d0da2b1cbaeb8ebafaa7cd75910daf0bd408ef0875ae5221b3227707f8d02a596b71d136cbfb457c208f17352d2d18bd2bbfe8107684f3f2fea3076926a10661406a942268beea32376c845212a3a8530b70f993f6526859093156ca098f2319998d4c17ca4cbad245123728c08d12315b50f8694cbd74ee9b49ab642fb87232d8bc95145be4e136ee9ce4b5955964ab9e8257a4cc19f394bd58229023052c0d73763892ea0c6f29bfff9b341ec3cc17890388d7e5b182356607ac2bb358f058ee5b64663397df9ce3fe9eeb21dfaea85b8a59fbf42e2f524ddbab34a146c40327c3a12bbff65e79465ae67eae4aa6145be50c561796d35b36b27916827",
        "a9064f024359a83f08a1cceda81473e2176735f4a9a0b2a2403f3e188b73998792a54d182ceb220b8715261a6f4c051548937d528f20203d56ffa85ca09f419f5d89447f0f595a73976e74c2fad651f3b6e93c563fa73da173809b0d9af2ef76702b0ebe516c30455750232982cf23f40fde64a4954142b75460fa0a24c18f88fbfc0f48a1eb75f1a7264c2b50222f89c8636f589250316137369f9b515030582635bdf9aabd28e0ff012810eab9f8fd59c479cdf5a2a555272f3185c052679e2bdbbe69c0b6fad3aa54399622a27a5cd347fee5bd0d820be2bfbf155df589826f039707e322807ade03fe8e7f6cb0d859a8f888547b2aac91535a2861ee25ab3268d2fcf49f8c4fdbdccc4182e68f712ae6e358fb0ed4d1d6020a1a72714b07d8c1f3c0635a3d6ea0096bc5bec63c4c8aa3573c5611a36e52a8c3bfded68f6bbc6d137e74a62fc7f73c13f53aa4a0c1348ca70aefea0309c4f692ac8074d0a77f6bebf79c9521747fe36650670b25432cf1008ecc59d51336e771955fd16390fb9b0326f45f95acb2f7e583c66bac472bb999c6e3589f00d656bd4cb266fb1c59d6aa673e1ab94fc30ce93cbf517af3bf6efa908a9cd8becf8add8d2bd7a10b892150985b5d7b4f9657efc225edf2828a0784bfd4cc91df815293c2aecf4fb99a71604ce31a054f0cf57ce25220da793a2c6005268a4f91e88d1b7c14030991",
        "10001",
        "929041da788439fbce845eaa856096cba9f77935da10da550ed8c803888888cca2406c05568ddd4b99aaa10bb30e7c29e470dfeec2e4dc8fd3ec84152dc26aa8c1f6d14a4c08f301e0818b05b7743843d88b8e224bb7bd1d4e8669ecf63b49a1adc9e852e74fa4dbbbc665770d52889e8d540e7918632a72c5dc0174e242653027307e20412767724fba49ba15164f9a6e04aafc6682db0ead4dd4405dd5e6d5c8ec38fb512e0d506c4e986b72de2825a7bd52d160f9d3db6ecae16a55461f4b6c44736c480b629c3fca20d52adb30b7650dbcdd817d4113055fb755d3cfa19954a20c486e8ff0509a4b646433e20fc6ca98564a00d77c3d7f1683cb187fe47091898e5b227826980055215008edd0c4e70313794b0d43edcad3e71d94e419bfab175f280be61915951168f6037fc6e7edbf03121a2537502ab2faa7c8fc86477be8b98e442c6f49ec6247478743947c980efbd3f0cca63aba0a998ca3ecc43d1b17100765c498b804c873dbc0dbb0726b5c37043bb93e5f0e5897f87f01d85b811e86fd517e33eca533b7ae92b40eaea46e47078517e06b2acccdf837b2d2081b584bbd8fc6e0d483886a27d94e9b6a73472dbab3faa710170a411efdb59b146ec5261cc95ff341a8951b4e778d5c9a42be29f35dfe98605dd50452e5a87081bc37682329cc51f07412f4d60febdbed7521ee3a194fb3b48edf541b0be0a071",
        "77481e96cd01f851426af3e2c2a12cbbf708c7b033a7d2d77e388578023fb1dd9b7b459b95c7b38888d2df4a850ff8cafbf9294c1ba8904e92e8a6e28d63421fa6aa620fd1024119713a26e921f4281ad22e078dd0485c5dc3d047f4ae506b15d922e33e5b6bdb716f0f9d4340435008bacbaa690e536e2d80e7ff87b4f5aa608444f244e03c081b7dd791ad28c34cc381c4f37efb3b2ec46f1107af8c92d50b9cbf524ad2f1e440fafad1b2265942181dfb4c4d8ba710342c50a2818a98135cfdbe2f5d6f2f16d11480d6a80b459ae52a869ba26d06f8e1678e2aa85978b6c8618ebcb820489f9f92021c9d47981450345903657ee1b43bdd8132f4ed29fd2cbcab6a9f2dd9834be44e306c187a8de45d6ce54cfd5d0eb8568d7967f1604d89fdf98e335d2574b65affe0360365603589cb5f5eb15ca75439ee2178d2ac1c08e8f064ba189cd5f394ad0130ed97f2a9a627d1dcb2965f997428cc05f6be2d803629c4d04bbca10c837d7eef216fd0c27441b7942edb42d061e6596e563b2c5bee1649200f057092ecd5a70139812019d5ed6e8bdd2dbdd706e3acece2964f76120b6eb7746e85bd87d67902adf40afdf18da3274a89f444a10d017a40bfb24eaf43ca2eb5a39f807cb4e4f63a91a10ec654ae6152b3192ac7f6fabb1654809076df5f8bd236f5f41c3f214ec3bac1befdafcb55aab2d0026dbe3a6100c29f5e",
        "3a7d38eb3ff20c0823bc559c142d248e420f88b077cab28ad642be2210b928b23a54d82a2f74b8e8c19168f1749be2841e493aece4753571389997a5d443040e1e4c34f655ed0872596f66679267651641ecf636ce79057c968d5310654ec768940cc0023edb5c10adf4f067c9d7efc87009c6e9e8c3f85708c5a1efc3e1562cd494d7922996ee8d1a83551e35df3062deebe796d246aef5bb52e3b9c19fa692cbeaa4605b6608b483228bff9670738fddd9a9ca67887283ae9a7926de6b695d016c713f9bd01a0b119a04280385a8f175d17738b8cbfc51cc46bc4760bc38dcfa073f0a1e7ab25a5f59af809fc0c89a6f2e4da90d3fb73a4a133d795cfa2e5440f3054d7640b3dd3f9d65168cd5ef3849f4fe53c26d220ec7619f2e35c144f4d53297e5e2af8b0a8dfd0d15d77f2e6067e100d90c3b0afdab37b6747b0c8f56dcc05b6706f4ed8ceb76286d05a6f7878b26269693ddf02f980e95f7f80a8780c1b676a562dab69f85cf83839ca09da9e83da19879e3ad9dcbb15846b071b259dfc8c88e317e46d62f0f1b45d36240c2dc970a0be128562ee9127fed1961530ba46168fd6019eefe8ec0c3f288af0a7a218b0db621d61f02d2540fb51f0840a9fda5a6407669cda41586cb105f2e2649068b14d997101212779295a03657f4ff14dae0eac339f1fcd5633e96708b1bff0c24619fa398fd8973792ad969f3e67f"
    },
    {
        0,
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
    }

};

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_rsaPublic(void);
static RunItError_t runIt_rsaPublicGeneral(const RunItRsaData_t *pData);
static RunItError_t runIt_rsaPrivate(void);
static RunItError_t runIt_rsaPrivateGeneral(const RunItRsaData_t *pData);
static RunItError_t runIt_rsaCheckPubpriv(void);
static RunItError_t runIt_rsaCheckPrivkey(void);
static RunItError_t runIt_rsaGenKey(void);
static RunItError_t runIt_rsaPkcsV21Test(void);
static RunItError_t runIt_rsaPkcsV21TestGeneral(const RunItRsaData_t *pData);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
RunItError_t runIt_buildPrivateKey(mbedtls_rsa_context* ctx)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_mpi p1, q1, h, g;

    mbedtls_mpi_init(&p1);
    mbedtls_mpi_init(&q1);
    mbedtls_mpi_init(&h);
    mbedtls_mpi_init(&g);

    RUNIT_ASSERT(mbedtls_mpi_sub_int(&p1, &ctx->P, 1) == 0);
    RUNIT_ASSERT(mbedtls_mpi_sub_int(&q1, &ctx->Q, 1) == 0);
    RUNIT_ASSERT(mbedtls_mpi_mul_mpi(&h, &p1, &q1) == 0);
    RUNIT_ASSERT(mbedtls_mpi_gcd(&g, &ctx->E, &h) == 0);
    RUNIT_ASSERT(mbedtls_mpi_inv_mod(&ctx->D, &ctx->E, &h) == 0);
    RUNIT_ASSERT(mbedtls_mpi_mod_mpi(&ctx->DP, &ctx->D, &p1) == 0);
    RUNIT_ASSERT(mbedtls_mpi_mod_mpi(&ctx->DQ, &ctx->D, &q1) == 0);
    RUNIT_ASSERT(mbedtls_mpi_inv_mod(&ctx->QP, &ctx->Q, &ctx->P) == 0);

bail:
    mbedtls_mpi_free(&p1);
    mbedtls_mpi_free(&q1);
    mbedtls_mpi_free(&h);
    mbedtls_mpi_free(&g);

    return rc;
}

static RunItError_t runIt_rsaPublic(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const RunItRsaData_t *iter = data;
    while (iter->mod != 0)
    {
        RUNIT_ASSERT(runIt_rsaPublicGeneral(iter) == RUNIT_ERROR__OK);
        iter++;
    }

bail:
    return rc;
}


static RunItError_t runIt_rsaPublicGeneral(const RunItRsaData_t *pData)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    static const uint32_t BUF_SIZE = 2048;

    static const char *MESSAGE_HEX_STRING = "59779fd2a39e56640c4fc1e67b60aeffcecd78aed7ad2bdfa464e93d04198d48466b8da7445f25bfa19db2844edd5c8f539cf772cc132b483169d390db28a43bc4ee0f038f6568ffc87447746cb72fefac2d6d90ee3143a915ac4688028805905a68eb8f8a96674b093c495eddd8704461eaa2b345efbb2ad6930acd8023f870";
    int mod = pData->mod;
    const char *name = pData->name;
    const char *input_N = pData->input_N;
    const char *input_E = pData->input_E;
    const char *result_hex_str = pData->public_result_hex_str;

    uint8_t *pMessageStr = NULL;
    uint8_t *pOutput = NULL;
    uint8_t *pOutputStr = NULL;
    mbedtls_rsa_context *pCtx = NULL;
    mbedtls_rsa_context *pCtx2 = NULL;

    RunItPtr messageStrPtr;
    RunItPtr outputPtr;
    RunItPtr outputStrPtr;
    RunItPtr ctxPtr;
    RunItPtr ctx2Ptr;

    mbedtls_mpi n;
    mbedtls_mpi e;

    const char* TEST_NAME = "RSA Public";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(messageStrPtr, pMessageStr, BUF_SIZE);
    ALLOC(outputPtr, pOutput, BUF_SIZE);
    ALLOC(outputStrPtr, pOutputStr, BUF_SIZE);
    ALLOC_STRUCT(mbedtls_rsa_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_rsa_context, ctx2Ptr, pCtx2);

    RUNIT_API(mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));
    RUNIT_API(mbedtls_rsa_init(pCtx2, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));

    memset(pMessageStr, 0, BUF_SIZE);
    memset(pOutput, 0, BUF_SIZE);
    memset(pOutputStr, 0, BUF_SIZE);

    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);

    pCtx->len = mod / 8;
    RUNIT_ASSERT(mbedtls_mpi_read_string(&n, 16, input_N) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&e, 16, input_E) == 0);

    RUNIT_ASSERT(mbedtls_rsa_import(pCtx, &n, NULL, NULL, NULL, &e) == 0);

    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_check_pubkey(pCtx) == 0);

    runIt_unhexify(pMessageStr, MESSAGE_HEX_STRING);

    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_public(pCtx, pMessageStr, pOutput) == 0);

    runIt_hexify(pOutputStr, pOutput, pCtx->len);

    RUNIT_ASSERT(strcasecmp((char *) pOutputStr, result_hex_str) == 0);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_copy(pCtx2, pCtx) == 0);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_check_pubkey(pCtx2) == 0);

    memset(pOutput, 0x00, BUF_SIZE);
    memset(pOutputStr, 0x00, BUF_SIZE);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_public(pCtx2, pMessageStr, pOutput) == 0);

    runIt_hexify(pOutputStr, pOutput, pCtx2->len);

    RUNIT_ASSERT(strcasecmp((char *) pOutputStr, result_hex_str) == 0);
bail:
    RUNIT_API(mbedtls_rsa_free(pCtx));
    RUNIT_API(mbedtls_rsa_free(pCtx2));

    FREE_IF_NOT_NULL(messageStrPtr);
    FREE_IF_NOT_NULL(outputPtr);
    FREE_IF_NOT_NULL(outputStrPtr);
    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctx2Ptr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] INPUT[%"PRIu32"B]",
                                   (uint32_t)mod, (uint32_t)strlen(MESSAGE_HEX_STRING));
    return rc;
}

static RunItError_t runIt_rsaPrivate(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const RunItRsaData_t *iter = data;
    while (iter->mod != 0)
    {
        RUNIT_ASSERT(runIt_rsaPrivateGeneral(iter) == RUNIT_ERROR__OK);
        iter++;
    }

bail:
    return rc;
}

static RunItError_t runIt_rsaPrivateGeneral(const RunItRsaData_t *pData)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    static const uint32_t BUF_SIZE = 2048;

    static const char *message_hex_string = "59779fd2a39e56640c4fc1e67b60aeffcecd78aed7ad2bdfa464e93d04198d48466b8da7445f25bfa19db2844edd5c8f539cf772cc132b483169d390db28a43bc4ee0f038f6568ffc87447746cb72fefac2d6d90ee3143a915ac4688028805905a68eb8f8a96674b093c495eddd8704461eaa2b345efbb2ad6930acd8023f870";
    int mod = pData->mod;
    const char *name = pData->name;
    const char *input_P = pData->input_P;
    const char *input_Q = pData->input_Q;
    const char *input_N = pData->input_N;
    const char *input_E = pData->input_E;
    const char *result_hex_str = pData->private_result_hex_str;

    uint8_t *pMessageStr = NULL;
    uint8_t *pOutput = NULL;
    uint8_t *pOutputStr = NULL;
    mbedtls_ctr_drbg_context *pCtrDrbg = (mbedtls_ctr_drbg_context *)gpRndState;
    mbedtls_rsa_context *pCtx = NULL;
    mbedtls_rsa_context *pCtx2 = NULL;

    RunItPtr messageStrPtr;
    RunItPtr outputPtr;
    RunItPtr outputStrPtr;
    RunItPtr ctxPtr;
    RunItPtr ctx2Ptr;
    int i;

    mbedtls_mpi p;
    mbedtls_mpi q;
    mbedtls_mpi n;
    mbedtls_mpi e;

    const char* TEST_NAME = "RSA Private";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(messageStrPtr, pMessageStr, BUF_SIZE);
    ALLOC(outputPtr, pOutput, BUF_SIZE);
    ALLOC(outputStrPtr, pOutputStr, BUF_SIZE);
    ALLOC_STRUCT(mbedtls_rsa_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_rsa_context, ctx2Ptr, pCtx2);

    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);

    RUNIT_API(mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));
    RUNIT_API(mbedtls_rsa_init(pCtx2, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));

    memset(pMessageStr, 0x00, 1000);

    pCtx->len = mod / 8;
    RUNIT_ASSERT(mbedtls_mpi_read_string(&p, 16, input_P) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&q, 16, input_Q) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&n, 16, input_N) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&e, 16, input_E) == 0);

    RUNIT_ASSERT(mbedtls_rsa_import(pCtx, &n, &p, &q, NULL, &e) == 0);

    RUNIT_ASSERT(runIt_buildPrivateKey(pCtx) == RUNIT_ERROR__OK);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_check_privkey(pCtx) == 0);

    runIt_unhexify(pMessageStr, message_hex_string);

    /* repeat three times to test updating of blinding values */
    for (i = 0; i < 3; i++)
    {
        memset(pOutput, 0x00, BUF_SIZE);
        memset(pOutputStr, 0x00, BUF_SIZE);

        RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_private(pCtx, mbedtls_ctr_drbg_random, pCtrDrbg, pMessageStr, pOutput) == 0);

        RUNIT_PRINT_BUF(pOutput, pCtx->len, "pOutput");

        runIt_hexify(pOutputStr, pOutput, pCtx->len);

        RUNIT_ASSERT(strcasecmp((char *) pOutputStr, result_hex_str) == 0);
    }

    /* And now one more time with the copy */
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_copy(pCtx2, pCtx) == 0);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_check_privkey(pCtx2) == 0);

    memset(pOutput, 0x00, BUF_SIZE);
    memset(pOutputStr, 0x00, BUF_SIZE);

    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_private(pCtx2, mbedtls_ctr_drbg_random, pCtrDrbg, pMessageStr, pOutput) == 0);

    runIt_hexify(pOutputStr, pOutput, pCtx2->len);

    RUNIT_ASSERT(strcasecmp((char *) pOutputStr, result_hex_str) == 0);

bail:
    RUNIT_API(mbedtls_rsa_free(pCtx));
    RUNIT_API(mbedtls_rsa_free(pCtx2));

    FREE_IF_NOT_NULL(messageStrPtr);
    FREE_IF_NOT_NULL(outputPtr);
    FREE_IF_NOT_NULL(outputStrPtr);
    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctx2Ptr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] INPUT[%"PRIu32"B]",
                                   (uint32_t)mod, (uint32_t)strlen(message_hex_string));
    return rc;
}

static RunItError_t runIt_rsaCheckPubpriv(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static const int mod = 2048;
    static const int radix_Npub = 16;
    static const char *input_Npub = "b38ac65c8141f7f5c96e14470e851936a67bf94cc6821a39ac12c05f7c0b06d9e6ddba2224703b02e25f31452f9c4a8417b62675fdc6df46b94813bc7b9769a892c482b830bfe0ad42e46668ace68903617faf6681f4babf1cc8e4b0420d3c7f61dc45434c6b54e2c3ee0fc07908509d79c9826e673bf8363255adb0add2401039a7bcd1b4ecf0fbe6ec8369d2da486eec59559dd1d54c9b24190965eafbdab203b35255765261cd0909acf93c3b8b8428cbb448de4715d1b813d0c94829c229543d391ce0adab5351f97a3810c1f73d7b1458b97daed4209c50e16d064d2d5bfda8c23893d755222793146d0a78c3d64f35549141486c3b0961a7b4c1a2034f";
    static const int radix_Epub = 16;
    static const char *input_Epub = "3";
    static const int radix_P = 16;
    static const char *input_P = "e79a373182bfaa722eb035f772ad2a9464bd842de59432c18bbab3a7dfeae318c9b915ee487861ab665a40bd6cda560152578e8579016c929df99fea05b4d64efca1d543850bc8164b40d71ed7f3fa4105df0fb9b9ad2a18ce182c8a4f4f975bea9aa0b9a1438a27a28e97ac8330ef37383414d1bd64607d6979ac050424fd17";
    static const int radix_Q = 16;
    static const char *input_Q = "c6749cbb0db8c5a177672d4728a8b22392b2fc4d3b8361d5c0d5055a1b4e46d821f757c24eef2a51c561941b93b3ace7340074c058c9bb48e7e7414f42c41da4cccb5c2ba91deb30c586b7fb18af12a52995592ad139d3be429add6547e044becedaf31fa3b39421e24ee034fbf367d11f6b8f88ee483d163b431e1654ad3e89";
    static const int radix_N = 16;
    static const char *input_N = "b38ac65c8141f7f5c96e14470e851936a67bf94cc6821a39ac12c05f7c0b06d9e6ddba2224703b02e25f31452f9c4a8417b62675fdc6df46b94813bc7b9769a892c482b830bfe0ad42e46668ace68903617faf6681f4babf1cc8e4b0420d3c7f61dc45434c6b54e2c3ee0fc07908509d79c9826e673bf8363255adb0add2401039a7bcd1b4ecf0fbe6ec8369d2da486eec59559dd1d54c9b24190965eafbdab203b35255765261cd0909acf93c3b8b8428cbb448de4715d1b813d0c94829c229543d391ce0adab5351f97a3810c1f73d7b1458b97daed4209c50e16d064d2d5bfda8c23893d755222793146d0a78c3d64f35549141486c3b0961a7b4c1a2034f";
    static const int radix_E = 16;
    static const char *input_E = "3";
    static const int radix_D = 16;
    static const char *input_D = "77B1D99300D6A54E864962DA09AE10CF19A7FB888456BC2672B72AEA52B204914493D16C184AD201EC3F762E1FBD8702BA796EF953D9EA2F26300D285264F11B0C8301D0207FEB1E2C984445C899B0ACEBAA74EF014DD1D4BDDB43202C08D2FF9692D8D788478DEC829EB52AFB5AE068FBDBAC499A27FACECC391E75C936D55F07BB45EE184DAB45808E15722502F279F89B38C1CB292557E5063597F52C75D61001EDC33F4739353E33E56AD273B067C1A2760208529EA421774A5FFFCB3423B1E0051E7702A55D80CBF2141569F18F87BFF538A1DA8EDBB2693A539F68E0D62D77743F89EACF3B1723BDB25CE2F333FA63CACF0E67DF1A431893BB9B352FCB";

    mbedtls_rsa_context *pPub = NULL;
    mbedtls_rsa_context *pPrv = NULL;

    RunItPtr pubPtr;
    RunItPtr prvPtr;

    mbedtls_mpi pub_n;
    mbedtls_mpi pub_e;

    mbedtls_mpi priv_p;
    mbedtls_mpi priv_q;
    mbedtls_mpi priv_n;
    mbedtls_mpi priv_e;
    mbedtls_mpi priv_d;

    const char* TEST_NAME = "RSA Check Public Private";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_rsa_context, pubPtr, pPub);
    ALLOC_STRUCT(mbedtls_rsa_context, prvPtr, pPrv);

    RUNIT_API(mbedtls_rsa_init( pPub, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));
    RUNIT_API(mbedtls_rsa_init( pPrv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));

    pPub->len = mod / 8;
    pPrv->len = mod / 8;

    mbedtls_mpi_init(&pub_n);
    mbedtls_mpi_init(&pub_e);
    mbedtls_mpi_init(&priv_p);
    mbedtls_mpi_init(&priv_q);
    mbedtls_mpi_init(&priv_n);
    mbedtls_mpi_init(&priv_e);
    mbedtls_mpi_init(&priv_d);

    RUNIT_ASSERT(mbedtls_mpi_read_string(&pub_n, radix_Npub, input_Npub) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&pub_e, radix_Epub, input_Epub) == 0);
    RUNIT_ASSERT(mbedtls_rsa_import(pPub, &pub_n,  NULL, NULL, NULL, &pub_e) == 0);

    RUNIT_ASSERT(mbedtls_mpi_read_string(&priv_p, radix_P, input_P) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&priv_q, radix_Q, input_Q) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&priv_n, radix_N, input_N) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&priv_e, radix_E, input_E) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&priv_d, radix_D, input_D) == 0);
    RUNIT_ASSERT(mbedtls_rsa_import(pPrv, &priv_n,  &priv_p, &priv_q, &priv_d, &priv_e) == 0);
    RUNIT_ASSERT(mbedtls_rsa_complete(pPrv) == 0);

    RUNIT_ASSERT_API(mbedtls_rsa_check_pub_priv(pPub, pPrv) == 0);

bail:
    RUNIT_API(mbedtls_rsa_free(pPub));
    RUNIT_API(mbedtls_rsa_free(pPrv));

    FREE_IF_NOT_NULL(pubPtr);
    FREE_IF_NOT_NULL(prvPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b]", (uint32_t)mod);
    return (rc);
}

static RunItError_t runIt_rsaCheckPrivkey(void)
{
    static const int mod = 2048;
    static const int radix_P = 16;
    static const char *input_P = "e79a373182bfaa722eb035f772ad2a9464bd842de59432c18bbab3a7dfeae318c9b915ee487861ab665a40bd6cda560152578e8579016c929df99fea05b4d64efca1d543850bc8164b40d71ed7f3fa4105df0fb9b9ad2a18ce182c8a4f4f975bea9aa0b9a1438a27a28e97ac8330ef37383414d1bd64607d6979ac050424fd17";
    static const int radix_Q = 16;
    static const char *input_Q = "c6749cbb0db8c5a177672d4728a8b22392b2fc4d3b8361d5c0d5055a1b4e46d821f757c24eef2a51c561941b93b3ace7340074c058c9bb48e7e7414f42c41da4cccb5c2ba91deb30c586b7fb18af12a52995592ad139d3be429add6547e044becedaf31fa3b39421e24ee034fbf367d11f6b8f88ee483d163b431e1654ad3e89";
    static const int radix_N = 16;
    static const char *input_N = "b38ac65c8141f7f5c96e14470e851936a67bf94cc6821a39ac12c05f7c0b06d9e6ddba2224703b02e25f31452f9c4a8417b62675fdc6df46b94813bc7b9769a892c482b830bfe0ad42e46668ace68903617faf6681f4babf1cc8e4b0420d3c7f61dc45434c6b54e2c3ee0fc07908509d79c9826e673bf8363255adb0add2401039a7bcd1b4ecf0fbe6ec8369d2da486eec59559dd1d54c9b24190965eafbdab203b35255765261cd0909acf93c3b8b8428cbb448de4715d1b813d0c94829c229543d391ce0adab5351f97a3810c1f73d7b1458b97daed4209c50e16d064d2d5bfda8c23893d755222793146d0a78c3d64f35549141486c3b0961a7b4c1a2034f";
    static const int radix_E = 16;
    static const char *input_E = "3";
    static const int radix_D = 16;
    static const char *input_D = "77B1D99300D6A54E864962DA09AE10CF19A7FB888456BC2672B72AEA52B204914493D16C184AD201EC3F762E1FBD8702BA796EF953D9EA2F26300D285264F11B0C8301D0207FEB1E2C984445C899B0ACEBAA74EF014DD1D4BDDB43202C08D2FF9692D8D788478DEC829EB52AFB5AE068FBDBAC499A27FACECC391E75C936D55F07BB45EE184DAB45808E15722502F279F89B38C1CB292557E5063597F52C75D61001EDC33F4739353E33E56AD273B067C1A2760208529EA421774A5FFFCB3423B1E0051E7702A55D80CBF2141569F18F87BFF538A1DA8EDBB2693A539F68E0D62D77743F89EACF3B1723BDB25CE2F333FA63CACF0E67DF1A431893BB9B352FCB";

    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_rsa_context *pCtx = NULL;

    RunItPtr ctxPtr;

    mbedtls_mpi p;
    mbedtls_mpi q;
    mbedtls_mpi n;
    mbedtls_mpi e;
    mbedtls_mpi d;

    const char* TEST_NAME = "RSA Check Private";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_rsa_context, ctxPtr, pCtx);

    RUNIT_API(mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));

    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&d);

    pCtx->len = mod / 8;
    RUNIT_ASSERT(mbedtls_mpi_read_string(&p, radix_P, input_P) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&q, radix_Q, input_Q) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&n, radix_N, input_N) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&e, radix_E, input_E) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&d, radix_D, input_D) == 0);
    RUNIT_ASSERT(mbedtls_rsa_import(pCtx, &n, &p, &q, &d, &e) == 0);
    RUNIT_ASSERT(mbedtls_rsa_complete(pCtx) == 0);

    RUNIT_ASSERT_API(mbedtls_rsa_check_privkey(pCtx) == 0);

bail:
    RUNIT_API(mbedtls_rsa_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b]", (uint32_t)mod);
    return (rc);

}

static RunItError_t runIt_rsaGenKey(void)
{
    const int nrbits = 2048;
    const int exponent = 3;

    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_rsa_context *pCtx = NULL;

    RunItPtr ctxPtr;

    const char* TEST_NAME = "RSA Gen Key";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_rsa_context, ctxPtr, pCtx);

    RUNIT_API(mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE));

    RUNIT_ASSERT_API(mbedtls_rsa_gen_key(pCtx, gpRndContext->rndGenerateVectFunc, gpRndState, nrbits, exponent) == 0);
    RUNIT_ASSERT_API(mbedtls_rsa_check_privkey(pCtx) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pCtx->P, &pCtx->Q) > 0);

bail:
    RUNIT_API(mbedtls_rsa_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
    return (rc);
}


static RunItError_t runIt_rsaPkcsV21Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const RunItRsaData_t *iter = data;
    while (iter->mod != 0)
    {
        RUNIT_ASSERT(runIt_rsaPkcsV21TestGeneral(iter) == RUNIT_ERROR__OK);
        iter++;
    }

bail:
    return rc;
}

static RunItError_t runIt_rsaPkcsV21TestGeneral(const RunItRsaData_t *pData)
{
    const uint32_t KEY_LEN = pData->mod;
    const char * RSA_P = pData->input_P;
    const char * RSA_Q = pData->input_Q;
    const char * RSA_N = pData->input_N;
    const char * RSA_E = pData->input_E;
    const char * RSA_D = pData->input_D;
    const char *name = pData->name;

    static const uint32_t PT_LEN = 40;
    static const char * RSA_PT = "385387514deccc7c740dd8cdf9daee49a1cbfd54";

    const char* TEST_NAME = "OAEP: RSAES + RSASSA";
    RunItError_t rc = RUNIT_ERROR__OK;


#if defined(MBEDTLS_PKCS1_V21)
    mbedtls_ctr_drbg_context *pCtrDrbg = (mbedtls_ctr_drbg_context *)gpRndState;
    mbedtls_rsa_context *pCtx = NULL;
    unsigned char *pRsaPlaintext = NULL;
    unsigned char *pRsaDecrypted = NULL;
    unsigned char *pRsaCiphertext = NULL;

    RunItPtr ctxPtr;
    RunItPtr rsaPlaintextPtr;
    RunItPtr rsaDecryptedPtr;
    RunItPtr rsaCiphertextPtr;

    size_t len;

    mbedtls_mpi p;
    mbedtls_mpi q;
    mbedtls_mpi n;
    mbedtls_mpi e;
    mbedtls_mpi d;

#if defined(MBEDTLS_SHA1_C)
    unsigned char *pSha1Sum = NULL;

    RunItPtr sha1SumPtr;
#endif

    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_rsa_context, ctxPtr, pCtx);
    ALLOC(rsaPlaintextPtr, pRsaPlaintext, PT_LEN);
    ALLOC(rsaDecryptedPtr, pRsaDecrypted, PT_LEN);
    ALLOC(rsaCiphertextPtr, pRsaCiphertext, KEY_LEN / 8);
#if defined(MBEDTLS_SHA1_C)
    ALLOC(sha1SumPtr, pSha1Sum, 20);
#endif

    RUNIT_API(mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1));

    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&d);

    pCtx->len = KEY_LEN / 8;
    RUNIT_ASSERT(mbedtls_mpi_read_string(&p, 16, RSA_P) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&q, 16, RSA_Q) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&n, 16, RSA_N) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&e, 16, RSA_E) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&d, 16, RSA_D) == 0);

    RUNIT_ASSERT(mbedtls_rsa_import(pCtx, &n, &p, &q, &d, &e) == 0);
    RUNIT_ASSERT(mbedtls_rsa_complete(pCtx) == 0);

    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_check_pubkey(pCtx) == 0);

    len = runIt_unhexify(pRsaPlaintext, RSA_PT);

    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_pkcs1_encrypt(pCtx, mbedtls_ctr_drbg_random, pCtrDrbg, MBEDTLS_RSA_PUBLIC, len, pRsaPlaintext, pRsaCiphertext) == 0);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_pkcs1_decrypt(pCtx, mbedtls_ctr_drbg_random, pCtrDrbg, MBEDTLS_RSA_PRIVATE, &len, pRsaCiphertext, pRsaDecrypted, PT_LEN) == 0);

    RUNIT_PRINT_BUF(pRsaPlaintext, len, "rsa_plaintext");
    RUNIT_PRINT_BUF(pRsaDecrypted, len, "rsa_decrypted");

    RUNIT_ASSERT(memcmp(pRsaDecrypted, pRsaPlaintext, len) == 0);

#if defined(MBEDTLS_SHA1_C)
    RUNIT_API(mbedtls_sha1(pRsaPlaintext, PT_LEN, pSha1Sum));
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_pkcs1_sign(pCtx, mbedtls_ctr_drbg_random, pCtrDrbg, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA1, 0, pSha1Sum, pRsaCiphertext) == 0);
    RUNIT_ASSERT_W_PARAM(name, mbedtls_rsa_pkcs1_verify(pCtx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA1, 0, pSha1Sum, pRsaCiphertext) == 0);
#endif /* MBEDTLS_SHA1_C */

#else /* MBEDTLS_PKCS1_V21 */
    RUNIT_PRINT_DBG("Test not implemented\n");
#endif /* MBEDTLS_PKCS1_V21 */

bail:
#if defined(MBEDTLS_PKCS1_V21)
    RUNIT_API(mbedtls_rsa_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(rsaPlaintextPtr);
    FREE_IF_NOT_NULL(rsaDecryptedPtr);
    FREE_IF_NOT_NULL(rsaCiphertextPtr);
#if defined(MBEDTLS_SHA1_C)
    FREE_IF_NOT_NULL(sha1SumPtr);
#endif /* defined(MBEDTLS_SHA1_C) */

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] PLAIN[%"PRIu32"B]",
                                   (uint32_t)KEY_LEN, (uint32_t)PT_LEN);
#endif /* defined(MBEDTLS_PKCS1_V21) */
    return (rc);
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_rsaTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "RSA";
    RUNIT_TEST_START(TEST_NAME);

    /* to suppress compiler warning when commenting out some of the test below */
    (void)runIt_rsaPublic;
    (void)runIt_rsaPrivate;
    (void)runIt_rsaCheckPubpriv;
    (void)runIt_rsaCheckPrivkey;
    (void)runIt_rsaPkcsV21Test;
    (void)runIt_rsaGenKey;

    RUNIT_ASSERT(runIt_rsaPublic() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_rsaPrivate() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_rsaCheckPubpriv() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_rsaCheckPrivkey() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_rsaPkcsV21Test() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_rsaGenKey() == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return (rc);
}

#endif /* MBEDTLS_RSA_C */
