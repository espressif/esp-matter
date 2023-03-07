function decryptImage() {
    var keyFile = $("#TOOLS_PROJ_IMAGE_KEY_FILE_NAME")[0];
    if (keyFile.files.length) {
        keyFile = keyFile.files[0];
    } else if (g_tools_img_dec_key_file) {
        keyFile = g_tools_img_dec_key_file;
    } else {
        fileBrowser.showInputDialog('No key file selected', 
                    false, 
                    null,
                    {text: "Close"});
            return;
    }
    
    var formData = new FormData()
    formData.append("key_file", keyFile)
    
    progressModalOpen("Activating Image", false);
        
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                getProgressStatus(100, null);
            } else if (data && data.err) {
                progressModalError("Image Activating Error:" + data.err);
            } else {
                progressModalError("Image Activating Error");
            }
        }
    }
        
    xhr.open("post", "/api/1/flash/activateImage", true);
    xhr.send(formData);    
}
