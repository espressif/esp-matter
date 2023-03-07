const modDef = {
	displayName: "RegDomain",
	config: [{
		name: "type",
		displayName: "Type",
		default: "2_4Ghz",
		options: [
			{ name: "2_4Ghz", displayName: "2.4Ghz" },
			{ name: "5_0Ghz", displayName: "5.0Ghz" }],
		onChange: (inst, ui) => {
			if (inst.type === "2_4Ghz") {
				ui.style2.hidden = false;
				ui.style5.hidden = true;
			} else {
				ui.style2.hidden = true;
				ui.style5.hidden = false;
			}
		}
	}, {
		name: "style2",
		displayName: "Style",
		default: "Recommended",
		options: [
			{ name: "A" },
			{ name: "B" },
			{ name: "C" },
			{ name: "D" },
			{ name: "E" },
			{ name: "F" },
			{ name: "Recommended" },
		]
	}, {
		name: "style5",
		displayName: "Style",
		default: "Recommended",
		options: [
			{ name: "A" },
			{ name: "B" },
			{ name: "C" },
			{ name: "Recommended" },
		],
		hidden: true
	}],
	moduleInstances: (inst) => {

		let moduleName = "/ti/drivers/net/imagecreator/new_design/WiFi/radio/" + inst.type + "/";
		let moduleName = "";
		if (inst.type === "2_4Ghz") {
			moduleName += inst.style2;
			//moduleName = "/ti/drivers/net/imagecreator/new_design/WiFi/regDomain2";
		} else {
			moduleName += inst.style5;
		}

		return [{
			name: "Example",
			moduleName
		}];
	}
};

exports = modDef;