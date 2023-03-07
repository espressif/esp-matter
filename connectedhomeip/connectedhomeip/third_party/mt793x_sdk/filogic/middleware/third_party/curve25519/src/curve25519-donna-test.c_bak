/*
	File:    curve25519-donna-test.c
	Package: WACServer
	Version: WAC_POSIX_Server_1.20
	
	Disclaimer: IMPORTANT: This Apple software is supplied to you, by Apple Inc. ("Apple"), in your
	capacity as a current, and in good standing, Licensee in the MFi Licensing Program. Use of this
	Apple software is governed by and subject to the terms and conditions of your MFi License,
	including, but not limited to, the restrictions specified in the provision entitled ”Public
	Software”, and is further subject to your agreement to the following additional terms, and your
	agreement that the use, installation, modification or redistribution of this Apple software
	constitutes acceptance of these additional terms. If you do not agree with these additional terms,
	please do not use, install, modify or redistribute this Apple software.
	
	Subject to all of these terms and in consideration of your agreement to abide by them, Apple grants
	you, for as long as you are a current and in good-standing MFi Licensee, a personal, non-exclusive
	license, under Apple's copyrights in this original Apple software (the "Apple Software"), to use,
	reproduce, and modify the Apple Software in source form, and to use, reproduce, modify, and
	redistribute the Apple Software, with or without modifications, in binary form. While you may not
	redistribute the Apple Software in source form, should you redistribute the Apple Software in binary
	form, you must retain this notice and the following text and disclaimers in all such redistributions
	of the Apple Software. Neither the name, trademarks, service marks, or logos of Apple Inc. may be
	used to endorse or promote products derived from the Apple Software without specific prior written
	permission from Apple. Except as expressly stated in this notice, no other rights or licenses,
	express or implied, are granted by Apple herein, including but not limited to any patent rights that
	may be infringed by your derivative works or by other works in which the Apple Software may be
	incorporated.
	
	Unless you explicitly state otherwise, if you provide any ideas, suggestions, recommendations, bug
	fixes or enhancements to Apple in connection with this software (“Feedback”), you hereby grant to
	Apple a non-exclusive, fully paid-up, perpetual, irrevocable, worldwide license to make, use,
	reproduce, incorporate, modify, display, perform, sell, make or have made derivative works of,
	distribute (directly or indirectly) and sublicense, such Feedback in connection with Apple products
	and services. Providing this Feedback is voluntary, but if you do provide Feedback to Apple, you
	acknowledge and agree that Apple may exercise the license granted above without the payment of
	royalties or further consideration to Participant.
	
	The Apple Software is provided by Apple on an "AS IS" basis. APPLE MAKES NO WARRANTIES, EXPRESS OR
	IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR
	IN COMBINATION WITH YOUR PRODUCTS.
	
	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
	AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
	(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
	
	Copyright (C) 2012-2013 Apple Inc. All Rights Reserved.
*/

#include "CommonServices.h"
#include "curve25519-donna.h"
#include "DebugServices.h"
#include "StringUtils.h"

//===========================================================================================================================
//	Internals
//===========================================================================================================================

OSStatus	curve25519_test( int print );
int			curve25519_djb_test( int print );

//===========================================================================================================================
//	Test Vectors
//===========================================================================================================================

typedef struct
{
	const char *		e;
	const char *		k;
	const char *		ek;
	
}	curve25519_test_vector;

static const curve25519_test_vector		kCurve25519TestVectors[] =
{
	// Spot checks from DJB's test-curve25519.c
	
	{ "0300000000000000000000000000000000000000000000000000000000000000", "0900000000000000000000000000000000000000000000000000000000000000", "2fe57da347cd62431528daac5fbb290730fff684afc4cfc2ed90995f58cb3b74" }, 
	{ "1d83942b17ef6c64a06fe99a5df8cdf79f9df3dc0efbe5ab9e3592e1abaa3a0c", "8ce8c87afba2b68da256294dd4fa13b0770bebccc333e83033ddf63857c2fe48", "93661bedaa09a9a38ad867711fc2591635500951d0adcdacd30d0bab736a1369" }, 
	{ "2406812eaa35b1840484ff1421d48b0cda169341aba7703f85af36e668cc081f", "c43e62490a7193b31002f25f68bc6f9c0ef63be1d7638720adb040a254ce1251", "248ffc90427e7791593063a0dc8808e959a4f146b8ea442403dbbcdc9cb43b05" }, 
	{ "a210f81ecec474f4d2953fdd35ebb120b850d2d3e6f0f4e77689e8235aa31972", "0885c109d88e6ad4a0f061af6dddb5311de8afde3e4425eb03fa9b9297df5c09", "34b92fe3fad307133db19671bce9203a4e77344bf5e71e051c31ec03c0c1b564" }, 
	{ "6009f9a6c360aea1612bffdb1db57e110446136e8af556a145b5db2607340d6e", "34215335a29b5abc1b3cea603363c968e09da02c6f0466b5f5d85486965f6d11", "07fa467d92b1633dd8115a969f00451f533f04f361c7d30ac977ec5958346d74" }, 
	{ "d33c825f56e68cab6b89f50c577d57275f65d53c8503eabe44cc741c27f9a141", "fa75f88f25c8d6e3ea47b240cb2996bc5d15252be814d656a5583b6e69f1e750", "fa0ee62eae0908821096b53a31d74b14baf18fdccb5596af15ab3833cc2fb730" }, 
	{ "3dfe8f3b9d7dc3cf83ea0f34813deb94744413b2e55a5fdff2dc53e52ab10477", "53589bb4cb01a0f88e0c3ecfa947f30cd80a251822287053e7e11eb0f48cf477", "17e5ee71b6e841f200bbf42083398e2d0955a17eeba15ad3a2d0b513ef628975" }, 
	{ "4d4cb8595db3197a604f0daf378afcec9f5e4da42dc7cf58609264be1467e449", "b0e0d1d4ebfdfc0764670926d3b8149779c4ba49adb9a94f3f6aae966da25a3f", "6652f2718e3209206ef7f83bc9ef369cf32c744d0c24c38dbc31d7df224cdb36" }, 
	{ "d27fe2796d7553e7c1522058f4166aa142818dc0b4eb0ee2334bbb090d199e52", "a6fcb3cc2ab400acd8a62148a643f5f8c70af2290bdb7c7b3f5422a135361137", "2db4bcf904e8e4d6c14ed3bb144b9b2065972fa280d0101b278a062cb0f20c7b" }, 
	{ "2f2d175b01bd42ab33e0b8e1096c97dc7038aa45b9f053fdea486ac2b85b7334", "30418647b212499ea7c11d74caa7a593023d67461e17c9097f50d399ce23c65e", "bc08f61cb8fb5167137018fb3ebcecd5bfd39b9c2294667ef34eaa8e4eb49f42" }, 
	{ "8d2747b9fb82788441f68511f8ccd880c2bb64bac2a468a29110bba165ad7e58", "f828d4826c2bab3e87e0f8426c2aafa2601f7fb5912a68e9b9d00a5fad46d567", "635378c8bbe30aff837086510f462067ce69f52a60690b2cd88ecfbb3c1a897b" }, 
	{ "10e54b6954fd891726e674bf70594e6406f46d706ed0774b9d684a25e141974c", "0fc661f488265ad677f81c7e0311d03dc97fcc0edced9686490db5cec67d2039", "3401a1b383c10b89693e82f36c85ab14449eac4adeeff4a8c941ca8126dabf1a" }, 
	{ "0a9250ed4995d161a19c03d8cfd465f603e5d3784e4038782099affa5050e70e", "90cba0168d3e24393d482634aa88e98f275857bb90c934d5b30708608439ca57", "bca9c321983e2f58362391258a3b03ed4595dfcfffa0fbeab97a996fc04cb222" }, 
	{ "49af81190869fd742a33691b0e0824d57e0329f4dd2819f5f32d130f1296b500", "7b2bcc18dab6706a24f22e4ccf6c174dba91915c83e5f04e51dae5201353da2f", "05aec13f92286f3a781ccae98995a3b9e0544770bc7de853b38f9100489e3e79" }, 
	{ "4faf81190869fd742a33691b0e0824d57e0329f4dd2819f5f32d130f1296b500", "05aec13f92286f3a781ccae98995a3b9e0544770bc7de853b38f9100489e3e79", "cd6e8269104eb5aaee886bd2071fba88bd13861475516bc2cd2b6e005e805064" }, 
	
	// Randomize tests
	
	{ "1c2b52c2b3c1e7f3f5b0caa1879c64c483503445f13411298e45705293ec31fa", "9414d5a5e714daae1df06a1226f2b327a9078c059aeb07bf3c761e702ce00b63", "1104c5480f67334864ca632ea7ea153ced82cbfab424f94de91323dba3127265" }, 
	{ "8a7293526883a21ab622a37d761c7fb7017a6d5edcfcb1099343d9845ac22c98", "0a37b22e9346c1beeb560aa057a11a409034f857cc4f9451f77eb0efd1e7f323", "5fd3ae641a33b91245bc38cbd9658a0139b0791f84b6037bc002c5570e126900" }, 
	{ "ceb0faf29061d17b97c6f82ba08a7aaf28c8a67271bb07a8f92dbde1d3927e39", "c46a5eff53953812fa4a3ac1e79ff36b9a3f89dc8912f99519c9804af8d7f2c8", "4e7f5959d22ec89402b26a89fd24727a7916cec0d33aa501bdd6ceee7c565301" }, 
	{ "8aa14303a04ed96fb9130b8942b621ff6e0294683de166a0cc9e45814bb5c6c2", "440e82fdd24e7eeeace02ce05bc5749c837b94afa11c7992709e7b8f37f17d8d", "c23bdfc35b8cbf1e81671c0e1c69cc54dec8b8fb9d5337eb734bb6c6904bea7b" }, 
	{ "046c57d634a77e8adb0ffef164a81f5cf460f398fc94d3393043ceac3fffec55", "77bb269d743891db779df634bcf64669e3dbdf398d50e6091eafb5eaf398904d", "e601d8b83d920ea1651d5ae4d61fe857a6c548c90c527eb7813f773d5f56616c" }, 
	{ "60cc524c63c5e1a42806c6b2c87f2a9959b3fe3c36df92fbd318974bd5acf937", "df8a95d651cefa88c2fe3d5dc8c8424cdfa3b4287b18ba6043c44e51a1366f07", "16951ed43ed9f30385714732797a1dbad687f4ae25238b7673790be88b352a1b" }, 
	{ "a28662f9c474f0269c9124c15dd2b2671be776a41a752aacfa79771fdf74248c", "7e895f55a59354dd8a976c071305bce3417904701c3ec0d83fd339093f7a55e7", "766349c4be8d1e528fbae8f9e62e99599b5ea850a50462f93f3e8f8eb6b2037e" }, 
	{ "04950a0366a91d3aed627bbd2ec67b1e1ba4b2372b382fe4b014b2667441407e", "b62bb87dd8e0a1ef98109d89490a887a76c0ff1a71dcc7f6d2632d30bfe92832", "51f4bc51519f88d77c31f44d957b63e79cd87f675808b3f7ac70d802412f1170" }, 
	{ "af1c3df1e36ca100786867be4061670d2cea031392a05f95894b5612077c54bb", "f53999a08f228bdeceb51db8601d966c1862599c4b09c14b7a4ad17ead75e242", "445077e1684740e0101dea615faf537e7b94ec53c01e6098d17136d16caeb534" }, 
	{ "fa324250b9885c182fc7d239a7951c19f94268918be20d11efcb9389f853bc22", "7a814d3e32c9d840a0f6c1e818e624c22aad859905e73cbbc1c79503f616a0f3", "2ffd39c944fd378fe91a81821796e2b953b43025df51ba021ec0882e84b87336" }, 
	{ "7cd3590a1879ab69fb5c5e135331a5b7e7f755faf226910634209a27baa41be4", "1077c6fed60e9f3a3debb5f81390d88c192aa58fac7f7b0480821b4b808fb311", "655cc324cc1214b89038fabe755e85afa4d8b2cb87f60053564733c61ea2e608" }, 
	{ "eb2346387b4f491d82a1971891906ca9881390459b8c16b2a6c6a4a1eb6685de", "01108b1ad6099f6a2f86efff866513a7d5830a7a4a35d83281a69ec125bbc21f", "cf8c6d82edfcc455a848ba8200ebb7407ac30b6025ca4f7185cc46b7dad6500f" }, 
	{ "c81cb0552083a78fc1b699c8c37b73ec6251b050cbc1a47393a98ac75d6215be", "7443afa103e6c4017e754c71944805210d9923cc6adced4fdf14a4a63136e597", "fde04b7fbfa46933743d3fc7c6bfd24546551054b6902d1051a615ed435d2f1b" }, 
	{ "a51e69117a0667f5224eacc99fe478aed2493ac335ee4d8fd890124337926798", "cb940253a4e410a40329c852283d9cd36b52b359044d4d287acc316dc2ff0360", "2d44884d17dd4dbab605096b638dd5ba9b9164a0acbfc8f6a06d68d5103fe327" }, 
	{ "fe18d34dcfcd7820d8c13fb43e6f40c20c157100ce17fdd7edf653a1520d3094", "57caba57e2fcebf7e123ec9a10daf2724b7cb15e82e6510640fdd10569491185", "3845428e1422b3e20b78b9602ce63c888d4fcbc28014bc0c8b99d668fe4d1b28" }, 
	{ "0e0134d0ed2af0542b5a7d78ce871673e51ec6c63442dd80d3cc7722580cf5f4", "667050b8aaf32e4f6c8e62cf703daac5ef7129df65b53b2b27ea41f93c141eaf", "076a2fd168e003c0ea78464ee91f951e6032d97ad5cd354e5b9d7ad2ff0c1f66" }, 
	{ "41d3fa481f3a13dd4c7ea207ca21a8b38c3f1fae3f19c5cf20603938ec2c5ed3", "51e37c2124c74fc50a9bc79f391dd40351ba33440500ac92c7b7cd4916195c08", "a0a1fc258c1b38b64c929d3c01fbf90671ffabc0fc7449b071a976a9d0d66436" }, 
	{ "5817dd367ff04c721930f798636756553add068c16036b8ac0d14bd1f23bcb82", "545f61446983972ab204340928446338d093b56d6483110319b26c8e6f93f32c", "16452ea127e9736ea6aea0869c27ae2ea350e2428f8cdfc51a230cadb6f13869" }, 
	{ "38749df62e572bc483128bc448730f780237a2d5664aa68b87812029b3fbda2b", "15922b29937e504b530c5e0ff72a466d4a9f0a635b8ff6a5415f4488d1617f50", "37ccbce3cfd85126b673384e0f372bba083bfd936c623f826010f77d96cecb70" }, 
	{ "4f076790f307004d51c6d39bec5cc95c1134425daf0f2f4b0c28806010c07f6b", "7da5e086671de010db9bb50b3ed8d90647fab1a0b92043c96c79d3feef3573cd", "fd27594f7e92a458450e239a4fc3c0fdb8e0e42a32527efcf8537e949919d413" }, 
	{ "9559a498d209ffaaa8b0f32590b9be85a1f4690a47d265800d802cbd2fee4eec", "4d1c565722c0a41dc05a8da242c124bc96a27829d2abac27ad5b2ae832c43ef2", "6fba4ce56c42c9f3f25698dda6c6461f5b753457e273d30e51a97381ea34fc0e" }, 
	{ "ce781076d55a6ff5495037d0030f751b78fa1a3c021e7038e893c6fe9898212a", "e2344d1091837a12b7cf0cc2810cf905e79395de139f9ebad459163e06e731bd", "1445e9dd8b9c7e057fb3f9eafa9e71624bb3f9fcc0fef5fbf6407039605d0164" }, 
	{ "4eb7a542327e1eda692d8e934e4547ce4a7bc4def3c671981d08f556c4cc19e9", "f49c9e9abcbb77ea386b85868700dd551fcadd3407a5e51159862d9dadc13c32", "972a275d2294c913a3c0a419300a51655bb90d9c8959708f02d957cacd46b67b" }, 
	{ "512a9c542eb4a267a01c3261390236d8b76c1d19087bc1daf722de60b687985b", "5e898b4b9f05f43ce9c77287d16aa5750b5e4bd15853ffd1f73bfddfc1e3e841", "518d8a5c1bc1e07cc88002f71838ac24fe388a1f6163c0091fcbf119f254ab6b" }, 
	{ "904dcb58db726df87a2cec2cc95a675263027aad5efdb08561ce5cd01a207e0b", "bee5579ce2afab4680ce8ede9a693589a97bacc9f3b5b309ad0bace249b2cbac", "bbdc51292b84ffa852c4081c13a2ac8b4d41301922fa69da140721645238a000" }, 
	{ "8cb6fb9d08b5f0032e0eb816066646e3ada754fe551c46d2e00b96ff7786ee3e", "0c29dbe29ad42a421b36334a0361ed4807f46b338c41411e64c54560a60fdc8e", "bfe667370ba5993cfab83a5261985135b1a728d4992f863ab7a91e7178f7625f" }, 
	{ "d6557369d28a7debe05ee2e1357be76864a3620993f4312f171b4b3ce5937278", "7b944f4862893f02fd91b1b5b98f98ff1028ea15d2af9793190bb6aa29946802", "257b6375fdfb2624a9c69601a50ebadc99586a6e9b78cb9969f7ca8bad71fd15" }, 
	{ "1a3191f8afb19fa223b7f87aef35efa609acf46e6c1a26f858965588459481d7", "c54507caa6986f2c05fdd1adbc59b7add71df3c9cc6ae7d9349e833cb0540cde", "06a30b5206b344912ab6dc63e31193f93fe0e521a02b09f544c31b4c16c7734a" }, 
	{ "424c1bb4865d1f7c7f20884a8fca09a00eea4f107836c1b31ce32bf2b76e7cbd", "c8a8f9bf6d3b6ce6ab1e11878419b9f934a729261cca7286f32e996094a49b94", "b1dce4273d60d3ad3c0e14b46d041a4b87a71b4db73ac7ec6f6c91abbcae584c" }, 
	{ "e5d272b2c425d77e7c593d32e184049cdc19e70ce2a14959a64ea07b13cd7a02", "567ad2b14340a3d3f606f500f22d8aeddd1c7df814ee30c266bff3ffe6d09fa0", "967f75c7dbb30f85ec27c2cf2b52f39070c26c2390ce99b8657f52f8eca33034" }, 
	{ "8caddb64f9698fe8f069caa0dc546b2905e903518351612ae14934cfa32339c7", "62210d5f895b27ebb9697ce7dd6a0d15dab97bdf89e39c8761a5f9e6ac51fc3e", "00bf90b64532dd7aa7eca09bb386bb11c625488fc6d39003a2a642047f238971" }, 
	{ "8cb9a96697d3bebe355fb0f46b69f562e99ef63a6e4522f3b65fff59e0b97034", "ce7aed98dff9378e9824ed9e6f2e70aaaed77757e91b0a10bc20ebb12f8d4aca", "2a4e65d7c2c6f106cb09daa1a709bbe84e2697070fea28fe0bdcf45e7f975d57" }, 
	{ "0ac0e03cd4f8e3b9d15da901129fe8d6c74d69a348cd84096afc9f24fd5c92bd", "5b3f61459aec6c4b5db7c3ff92e3f5db593d400d8b29cbea6a8c67f6f1284991", "52828e857de6b2cd3667ee9f0deb61cb00029b51c4ab0bc39300e7395cb65b23" }, 
	
	// Edge cases
	
	{ "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000" }, 
	{ "5555555555555555555555555555555555555555555555555555555555555555", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "47f12c4206808bc58f18b31dc488240246e2422413f2ed535884a37828587a2a" }, 
	{ "0000000000000000000000000000000000000000000000000000000000000000", "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "b32d1362c248d62fe62619cff04dd43db73ffc1b6308ede30b78d87380f1e834" }, 
	{ "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000" }, 
	{ "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "96186d56afdbfeda62f0d07168fa8b142b3d8530e9705fd818cfd33591ea927f" }, 
	{ "1111111111111111111111111111111111111111111111111111111111111111", "1111111111111111111111111111111111111111111111111111111111111111", "cee12e7b10eeb556b7a1d5ee0f39ab8f5eb7fe2e3004d909c4567f4ea4ee6b48" }, 
	{ "2222222222222222222222222222222222222222222222222222222222222222", "2222222222222222222222222222222222222222222222222222222222222222", "84825d28d4210f30675e0bff314f0dbdddc16879d2ae21fd894bc31f59ea4a72" }, 
	{ "3333333333333333333333333333333333333333333333333333333333333333", "3333333333333333333333333333333333333333333333333333333333333333", "de0b3001d7c18f6ed60fa2ed16d3faab647f1fe4daaf821e408589a4c573b901" }, 
	{ "4444444444444444444444444444444444444444444444444444444444444444", "4444444444444444444444444444444444444444444444444444444444444444", "78030b67d77e9e587c795ef70e4c6f832c1443113f4dd7443c5e6fdb637b054a" }, 
	{ "5555555555555555555555555555555555555555555555555555555555555555", "5555555555555555555555555555555555555555555555555555555555555555", "918eeff05e4aebe5da204cdfccbc338f6e687dc48c4beecaa01753ab05aaa539" }, 
	{ "6666666666666666666666666666666666666666666666666666666666666666", "6666666666666666666666666666666666666666666666666666666666666666", "505fd4a10f552306930716815a062c5b5389db211f1cae751aded7c2a7b96333" }, 
	{ "7777777777777777777777777777777777777777777777777777777777777777", "7777777777777777777777777777777777777777777777777777777777777777", "b6b33579d186ee9edf8a910ea853ef968cf0d586425068910d4af18a2a9b2042" }, 
	{ "8888888888888888888888888888888888888888888888888888888888888888", "8888888888888888888888888888888888888888888888888888888888888888", "15e1cb50268c3cac080d48104847230b39ae55afb8cfcc29c3b5bdb347b1cd59" }, 
	{ "9999999999999999999999999999999999999999999999999999999999999999", "9999999999999999999999999999999999999999999999999999999999999999", "ba8d25500d7846ce4a4f4472eb379f829d256e79d5afb840868cf1299e7c4818" }, 
	{ "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "0089e9f5895eda9443fbd5441ecc7b3de6bf42da61ce752d1e1f77108420d503" }, 
	{ "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "efc3928960b8db5a9a001c600e3f8c75570bbd1f21a979b6731530f8ee8f1075" }, 
	{ "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc", "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc", "9ee8555ce05026d0b41a939a7b909a82fdb6b42682c386f2236dc27bb24c5b48" }, 
	{ "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", "9cbe42b2a868f446e8406f4316e086141c650df5434109d345fbb377e1997b0e" }, 
	{ "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", "3ebb6448e85a44c66345e2e295f8726e87f789e940dbb63da169b7a17c52b154" }, 
	{ "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "96186d56afdbfeda62f0d07168fa8b142b3d8530e9705fd818cfd33591ea927f" }, 
};

//===========================================================================================================================
//	curve25519_test
//===========================================================================================================================

OSStatus	curve25519_test( int print )
{
	OSStatus			err;
	uint8_t				e[ 32 ], k[ 32 ], ek[ 32 ], ek2[ 32 ];
	size_t				i, j, len;
	CFAbsoluteTime		t = 0;
	
	for( i = 0; i < countof( kCurve25519TestVectors ); ++i )
	{
		const curve25519_test_vector * const 	tv = &kCurve25519TestVectors[ i ];
		
		err = HexToData( tv->e, kSizeCString, kHexToData_NoFlags, e, sizeof( e ), &len, NULL, NULL );
		require_noerr( err, exit );
		require_action( len == 32, exit, err = kSizeErr );
		
		err = HexToData( tv->k, kSizeCString, kHexToData_NoFlags, k, sizeof( k ), &len, NULL, NULL );
		require_noerr( err, exit );
		require_action( len == 32, exit, err = kSizeErr );
		
		err = HexToData( tv->ek, kSizeCString, kHexToData_NoFlags, ek, sizeof( ek ), &len, NULL, NULL );
		require_noerr( err, exit );
		require_action( len == 32, exit, err = kSizeErr );
		
		memset( ek2, 0, sizeof( ek2 ) );
		curve25519_donna( ek2, e, k );
		require_action( memcmp( ek, ek2, 32 ) == 0, exit, err = kMismatchErr );
		
		if( print )
		{
			for( j = 0; j < 32; ++j ) printf( "%02x", e[ j ] );  printf( " " );
			for( j = 0; j < 32; ++j ) printf( "%02x", k[ j ] );  printf( " " );
			for( j = 0; j < 32; ++j ) printf( "%02x", ek[ j ] ); printf( "\n" );
		}
	}
	
	t = CFAbsoluteTimeGetCurrent();
	err = curve25519_djb_test( print );
	require_noerr( err, exit );
	t = CFAbsoluteTimeGetCurrent() - t;
	
exit:
	FPrintF( stdout, "%###s: %s (%f seconds)\n", __ROUTINE__, !err ? "PASSED" : "FAILED", t );
	return( err );
}

//===========================================================================================================================
//	curve25519_djb_test
//
//	From DJB's pubic domain test code, modified to let you turn off printing. Maintained his style here for diff'ing.
//===========================================================================================================================

static void curve25519_djb_doit(unsigned char *ek,unsigned char *e,unsigned char *k, int print)
{
  int i;

  if (print) {
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) e[i]); printf(" ");
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) k[i]); printf(" ");
  }
  curve25519_donna(ek,e,k);
  if (print) {
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) ek[i]); printf("\n");
  }
}

int curve25519_djb_test(int print)
{
  unsigned char e1k[32];
  unsigned char e2k[32];
  unsigned char e1e2k[32];
  unsigned char e2e1k[32];
  unsigned char e1[32] = {3};
  unsigned char e2[32] = {5};
  unsigned char k[32] = {9};
  int loop;
  int i;

  memset(e1k,0,sizeof(e1k));
  memset(e2k,0,sizeof(e2k));
  memset(e1e2k,0,sizeof(e1e2k));
  memset(e2e1k,0,sizeof(e2e1k));
  for (loop = 0;loop < 10000;++loop) {
    curve25519_djb_doit(e1k,e1,k, print);
    curve25519_djb_doit(e2e1k,e2,e1k, print);
    curve25519_djb_doit(e2k,e2,k, print);
    curve25519_djb_doit(e1e2k,e1,e2k, print);
    for (i = 0;i < 32;++i) if (e1e2k[i] != e2e1k[i]) {
      if (print) printf("fail\n");
      return 1;
    }
    for (i = 0;i < 32;++i) e1[i] ^= e2k[i];
    for (i = 0;i < 32;++i) e2[i] ^= e1k[i];
    for (i = 0;i < 32;++i) k[i] ^= e1e2k[i];
  }

  return 0;
}
