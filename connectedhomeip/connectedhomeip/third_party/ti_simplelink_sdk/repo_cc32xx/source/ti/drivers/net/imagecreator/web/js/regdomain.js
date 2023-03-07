

function loadFCC()
{
    {
        $('#rd_FCC_Ch36').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch40').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch44').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch48').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch52').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch56').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch60').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch64').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch144').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch149').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch153').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch157').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch161').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch165').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        $('#rd_FCC_Ch169').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
    }
    {
        $('#rd_FCC_Ch36').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_36 / 8);
        $('#rd_FCC_Ch40').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_40 / 8);
        $('#rd_FCC_Ch44').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_44 / 8);
        $('#rd_FCC_Ch48').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_48 / 8);
        $('#rd_FCC_Ch52').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_52 / 8);
        $('#rd_FCC_Ch56').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_56 / 8);
        $('#rd_FCC_Ch60').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_60 / 8);
        $('#rd_FCC_Ch64').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_64 / 8);
        $('#rd_FCC_Ch100').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_100 / 8);
        $('#rd_FCC_Ch104').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_104 / 8);
        $('#rd_FCC_Ch108').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_108 / 8);
        $('#rd_FCC_Ch112').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_112 / 8);
        $('#rd_FCC_Ch116').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_116 / 8);
        $('#rd_FCC_Ch120').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_120 / 8);
        $('#rd_FCC_Ch124').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_124 / 8);
        $('#rd_FCC_Ch128').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_128 / 8);
        $('#rd_FCC_Ch132').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_132 / 8);
        $('#rd_FCC_Ch136').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_136 / 8);
        $('#rd_FCC_Ch140').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_140 / 8);
        $('#rd_FCC_Ch144').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_144 / 8);
        $('#rd_FCC_Ch149').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_149 / 8);
        $('#rd_FCC_Ch153').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_153 / 8);
        $('#rd_FCC_Ch157').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_157 / 8);
        $('#rd_FCC_Ch161').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_161 / 8);
        $('#rd_FCC_Ch165').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_165 / 8);
        $('#rd_FCC_Ch169').val(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_169 / 8);
    }
    $("#regdomcbFCC_1").prop("checked", project.systemFiles.REG_DOMAIN.FCC_ENABLED);

    $('#regdomcbFCC_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.FCC_ENABLED = $('#regdomcbFCC_1').is(':checked')
        {
            $('#rd_FCC_Ch36').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch40').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch44').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch48').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch52').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch56').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch60').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch64').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch144').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch149').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch153').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch157').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch161').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch165').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
            $('#rd_FCC_Ch169').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC_ENABLED)
        }
        {
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_36 = Math.floor(Number($('#rd_FCC_Ch36').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_40 = Math.floor(Number($('#rd_FCC_Ch40').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_44 = Math.floor(Number($('#rd_FCC_Ch44').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_48 = Math.floor(Number($('#rd_FCC_Ch48').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_52 = Math.floor(Number($('#rd_FCC_Ch52').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_56 = Math.floor(Number($('#rd_FCC_Ch56').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_60 = Math.floor(Number($('#rd_FCC_Ch60').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_64 = Math.floor(Number($('#rd_FCC_Ch64').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_100 = Math.floor(Number($('#rd_FCC_Ch100').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_104 = Math.floor(Number($('#rd_FCC_Ch104').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_108 = Math.floor(Number($('#rd_FCC_Ch108').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_112 = Math.floor(Number($('#rd_FCC_Ch112').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_116 = Math.floor(Number($('#rd_FCC_Ch116').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_120 = Math.floor(Number($('#rd_FCC_Ch120').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_124 = Math.floor(Number($('#rd_FCC_Ch124').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_128 = Math.floor(Number($('#rd_FCC_Ch128').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_132 = Math.floor(Number($('#rd_FCC_Ch132').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_136 = Math.floor(Number($('#rd_FCC_Ch136').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_140 = Math.floor(Number($('#rd_FCC_Ch140').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_144 = Math.floor(Number($('#rd_FCC_Ch144').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_149 = Math.floor(Number($('#rd_FCC_Ch149').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_153 = Math.floor(Number($('#rd_FCC_Ch153').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_157 = Math.floor(Number($('#rd_FCC_Ch157').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_161 = Math.floor(Number($('#rd_FCC_Ch161').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_165 = Math.floor(Number($('#rd_FCC_Ch165').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC_CHANNEL_169 = Math.floor(Number($('#rd_FCC_Ch169').val()) * 8);
        }
        saveProjectAPI();

    });

}

function loadETCI() {
    //ETCI default
    {
        $('#rd_ETCI_Ch36').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch40').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch44').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch48').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch52').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch56').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch60').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch64').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)
        $('#rd_ETCI_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED)

        $('#rd_ETCI_Ch144').prop('disabled', true)
        $('#rd_ETCI_Ch149').prop('disabled', true)
        $('#rd_ETCI_Ch153').prop('disabled', true)
        $('#rd_ETCI_Ch157').prop('disabled', true)
        $('#rd_ETCI_Ch161').prop('disabled', true)
        $('#rd_ETCI_Ch165').prop('disabled', true)
        $('#rd_ETCI_Ch169').prop('disabled', true)
    }
    {
        $('#rd_ETCI_Ch36').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_36 / 8);
        $('#rd_ETCI_Ch40').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_40 / 8);
        $('#rd_ETCI_Ch44').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_44 / 8);
        $('#rd_ETCI_Ch48').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_48 / 8);
        $('#rd_ETCI_Ch52').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_52 / 8);
        $('#rd_ETCI_Ch56').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_56 / 8);
        $('#rd_ETCI_Ch60').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_60 / 8);
        $('#rd_ETCI_Ch64').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_64 / 8);
        $('#rd_ETCI_Ch100').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_100 / 8);
        $('#rd_ETCI_Ch104').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_104 / 8);
        $('#rd_ETCI_Ch108').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_108 / 8);
        $('#rd_ETCI_Ch112').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_112 / 8);
        $('#rd_ETCI_Ch116').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_116 / 8);
        $('#rd_ETCI_Ch120').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_120 / 8);
        $('#rd_ETCI_Ch124').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_124 / 8);
        $('#rd_ETCI_Ch128').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_128 / 8);
        $('#rd_ETCI_Ch132').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_132 / 8);
        $('#rd_ETCI_Ch136').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_136 / 8);
        $('#rd_ETCI_Ch140').val(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_140 / 8);
    }
    $("#regdomcbETCI_1").prop("checked", project.systemFiles.REG_DOMAIN.ETSI_ENABLED);

    $('#regdomcbETCI_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.ETSI_ENABLED = $('#regdomcbETCI_1').is(':checked')
        {
            $('#rd_ETCI_Ch36' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch40' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch44' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch48' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch52' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch56' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch60' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch64' ).prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI_ENABLED);
            $('#rd_ETCI_Ch144').prop('disabled', true);
            $('#rd_ETCI_Ch149').prop('disabled', true);
            $('#rd_ETCI_Ch153').prop('disabled', true);
            $('#rd_ETCI_Ch157').prop('disabled', true);
            $('#rd_ETCI_Ch161').prop('disabled', true);
            $('#rd_ETCI_Ch165').prop('disabled', true);
            $('#rd_ETCI_Ch169').prop('disabled', true);
        }
        {
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_36 = Math.floor(Number($('#rd_ETCI_Ch36').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_40 = Math.floor(Number($('#rd_ETCI_Ch40').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_44 = Math.floor(Number($('#rd_ETCI_Ch44').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_48 = Math.floor(Number($('#rd_ETCI_Ch48').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_52 = Math.floor(Number($('#rd_ETCI_Ch52').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_56 = Math.floor(Number($('#rd_ETCI_Ch56').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_60 = Math.floor(Number($('#rd_ETCI_Ch60').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_64 = Math.floor(Number($('#rd_ETCI_Ch64').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_100 = Math.floor(Number($('#rd_ETCI_Ch100').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_104 = Math.floor(Number($('#rd_ETCI_Ch104').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_108 = Math.floor(Number($('#rd_ETCI_Ch108').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_112 = Math.floor(Number($('#rd_ETCI_Ch112').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_116 = Math.floor(Number($('#rd_ETCI_Ch116').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_120 = Math.floor(Number($('#rd_ETCI_Ch120').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_124 = Math.floor(Number($('#rd_ETCI_Ch124').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_128 = Math.floor(Number($('#rd_ETCI_Ch128').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_132 = Math.floor(Number($('#rd_ETCI_Ch132').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_136 = Math.floor(Number($('#rd_ETCI_Ch136').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_140 = Math.floor(Number($('#rd_ETCI_Ch140').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_144 = Math.floor(Number($('#rd_ETCI_Ch144').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_149 = Math.floor(Number($('#rd_ETCI_Ch149').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_153 = Math.floor(Number($('#rd_ETCI_Ch153').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_157 = Math.floor(Number($('#rd_ETCI_Ch157').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_161 = Math.floor(Number($('#rd_ETCI_Ch161').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_165 = Math.floor(Number($('#rd_ETCI_Ch165').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_169 = Math.floor(Number($('#rd_ETCI_Ch169').val()) * 8);
        }
        saveProjectAPI();
    });


}

function loadJP() {
    //JP default
    {
        $('#rd_JP_Ch36').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch40').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch44').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch48').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch52').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch56').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch60').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch64').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
        $('#rd_JP_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);

        $('#rd_JP_Ch144').prop('disabled', true);
        $('#rd_JP_Ch149').prop('disabled', true);
        $('#rd_JP_Ch153').prop('disabled', true);
        $('#rd_JP_Ch157').prop('disabled', true);
        $('#rd_JP_Ch161').prop('disabled', true);
        $('#rd_JP_Ch165').prop('disabled', true);
        $('#rd_JP_Ch169').prop('disabled', true);
    }
    {
        $('#rd_JP_Ch36' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_36  / 8);
        $('#rd_JP_Ch40' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_40  / 8);
        $('#rd_JP_Ch44' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_44  / 8);
        $('#rd_JP_Ch48' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_48  / 8);
        $('#rd_JP_Ch52' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_52  / 8);
        $('#rd_JP_Ch56' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_56  / 8);
        $('#rd_JP_Ch60' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_60  / 8);
        $('#rd_JP_Ch64' ).val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_64  / 8);
        $('#rd_JP_Ch100').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_100 / 8);
        $('#rd_JP_Ch104').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_104 / 8);
        $('#rd_JP_Ch108').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_108 / 8);
        $('#rd_JP_Ch112').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_112 / 8);
        $('#rd_JP_Ch116').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_116 / 8);
        $('#rd_JP_Ch120').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_120 / 8);
        $('#rd_JP_Ch124').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_124 / 8);
        $('#rd_JP_Ch128').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_128 / 8);
        $('#rd_JP_Ch132').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_132 / 8);
        $('#rd_JP_Ch136').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_136 / 8);
        $('#rd_JP_Ch140').val(project.systemFiles.REG_DOMAIN.JP_CHANNEL_140 / 8);
    }

    $("#regdomcbJP_1").prop("checked", project.systemFiles.REG_DOMAIN.JP_ENABLED);

    $('#regdomcbJP_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.JP_ENABLED = $('#regdomcbJP_1').is(':checked')
        {
            $('#rd_JP_Ch36').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch40').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch44').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch48').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch52').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch56').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch60').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch64').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch100').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch104').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch108').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch112').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch116').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch120').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch124').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch128').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch132').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch136').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch140').prop('disabled', !project.systemFiles.REG_DOMAIN.JP_ENABLED);
            $('#rd_JP_Ch144').prop('disabled', true);
            $('#rd_JP_Ch149').prop('disabled', true);
            $('#rd_JP_Ch153').prop('disabled', true);
            $('#rd_JP_Ch157').prop('disabled', true);
            $('#rd_JP_Ch161').prop('disabled', true);
            $('#rd_JP_Ch165').prop('disabled', true);
            $('#rd_JP_Ch169').prop('disabled', true);
        }
        {
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_36 = Math.floor(Number($('#rd_JP_Ch36').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_40 = Math.floor(Number($('#rd_JP_Ch40').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_44 = Math.floor(Number($('#rd_JP_Ch44').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_48 = Math.floor(Number($('#rd_JP_Ch48').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_52 = Math.floor(Number($('#rd_JP_Ch52').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_56 = Math.floor(Number($('#rd_JP_Ch56').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_60 = Math.floor(Number($('#rd_JP_Ch60').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_64 = Math.floor(Number($('#rd_JP_Ch64').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_100 = Math.floor(Number($('#rd_JP_Ch100').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_104 = Math.floor(Number($('#rd_JP_Ch104').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_108 = Math.floor(Number($('#rd_JP_Ch108').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_112 = Math.floor(Number($('#rd_JP_Ch112').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_116 = Math.floor(Number($('#rd_JP_Ch116').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_120 = Math.floor(Number($('#rd_JP_Ch120').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_124 = Math.floor(Number($('#rd_JP_Ch124').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_128 = Math.floor(Number($('#rd_JP_Ch128').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_132 = Math.floor(Number($('#rd_JP_Ch132').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_136 = Math.floor(Number($('#rd_JP_Ch136').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_140 = Math.floor(Number($('#rd_JP_Ch140').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_144 = Math.floor(Number($('#rd_JP_Ch144').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_149 = Math.floor(Number($('#rd_JP_Ch149').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_153 = Math.floor(Number($('#rd_JP_Ch153').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_157 = Math.floor(Number($('#rd_JP_Ch157').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_161 = Math.floor(Number($('#rd_JP_Ch161').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_165 = Math.floor(Number($('#rd_JP_Ch165').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP_CHANNEL_169 = Math.floor(Number($('#rd_JP_Ch169').val()) * 8);
        }
        
        saveProjectAPI();
    });


}

function loadExtraBO() {
    //EBH default
    
    {
        $('#rd_BO_Ch36H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch40H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch44H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch48H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch52H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch56H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch60H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch64H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch100H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch104H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch108H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch112H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch116H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch120H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch124H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch128H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch132H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch136H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch140H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch144H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch149H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch153H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch157H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch161H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch165H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch169H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)

        $('#rd_BO_Ch36L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch40L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch44L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch48L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch52L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch56L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch60L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch64L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch100L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch104L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch108L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch112L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch116L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch120L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch124L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch128L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch132L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch136L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch140L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch144L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch149L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch153L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch157L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch161L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch165L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        $('#rd_BO_Ch169L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
    }

    {
        $('#rd_BO_Ch36H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_36  / 8);
        $('#rd_BO_Ch40H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_40  / 8);
        $('#rd_BO_Ch44H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_44  / 8);
        $('#rd_BO_Ch48H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_48  / 8);
        $('#rd_BO_Ch52H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_52  / 8);
        $('#rd_BO_Ch56H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_56  / 8);
        $('#rd_BO_Ch60H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_60  / 8);
        $('#rd_BO_Ch64H' ).val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_64  / 8);
        $('#rd_BO_Ch100H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_100 / 8);
        $('#rd_BO_Ch104H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_104 / 8);
        $('#rd_BO_Ch108H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_108 / 8);
        $('#rd_BO_Ch112H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_112 / 8);
        $('#rd_BO_Ch116H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_116 / 8);
        $('#rd_BO_Ch120H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_120 / 8);
        $('#rd_BO_Ch124H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_124 / 8);
        $('#rd_BO_Ch128H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_128 / 8);
        $('#rd_BO_Ch132H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_132 / 8);
        $('#rd_BO_Ch136H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_136 / 8);
        $('#rd_BO_Ch140H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_140 / 8);
        $('#rd_BO_Ch144H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_144 / 8);
        $('#rd_BO_Ch149H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_149 / 8);
        $('#rd_BO_Ch153H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_153 / 8);
        $('#rd_BO_Ch157H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_157 / 8);
        $('#rd_BO_Ch161H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_161 / 8);
        $('#rd_BO_Ch165H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_165 / 8);
        $('#rd_BO_Ch169H').val(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_169 / 8);

        $('#rd_BO_Ch36L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_36  / 8);
        $('#rd_BO_Ch40L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_40  / 8);
        $('#rd_BO_Ch44L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_44  / 8);
        $('#rd_BO_Ch48L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_48  / 8);
        $('#rd_BO_Ch52L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_52  / 8);
        $('#rd_BO_Ch56L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_56  / 8);
        $('#rd_BO_Ch60L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_60  / 8);
        $('#rd_BO_Ch64L' ).val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_64  / 8);
        $('#rd_BO_Ch100L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_100 / 8);
        $('#rd_BO_Ch104L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_104 / 8);
        $('#rd_BO_Ch108L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_108 / 8);
        $('#rd_BO_Ch112L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_112 / 8);
        $('#rd_BO_Ch116L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_116 / 8);
        $('#rd_BO_Ch120L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_120 / 8);
        $('#rd_BO_Ch124L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_124 / 8);
        $('#rd_BO_Ch128L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_128 / 8);
        $('#rd_BO_Ch132L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_132 / 8);
        $('#rd_BO_Ch136L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_136 / 8);
        $('#rd_BO_Ch140L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_140 / 8);
        $('#rd_BO_Ch144L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_144 / 8);
        $('#rd_BO_Ch149L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_149 / 8);
        $('#rd_BO_Ch153L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_153 / 8);
        $('#rd_BO_Ch157L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_157 / 8);
        $('#rd_BO_Ch161L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_161 / 8);
        $('#rd_BO_Ch165L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_165 / 8);
        $('#rd_BO_Ch169L').val(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_169 / 8);
    }

    $("#regdomcbBO_1").prop("checked", project.systemFiles.REG_DOMAIN.ExBO_ENABLED);

    $('#regdomcbBO_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.ExBO_ENABLED = $('#regdomcbBO_1').is(':checked')
        {
            $('#rd_BO_Ch36H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch40H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch44H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch48H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch52H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch56H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch60H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch64H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch100H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch104H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch108H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch112H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch116H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch120H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch124H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch128H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch132H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch136H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch140H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch144H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch149H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch153H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch157H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch161H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch165H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch169H').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)

            $('#rd_BO_Ch36L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch40L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch44L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch48L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch52L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch56L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch60L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch64L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch100L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch104L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch108L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch112L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch116L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch120L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch124L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch128L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch132L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch136L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch140L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch144L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch149L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch153L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch157L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch161L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch165L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
            $('#rd_BO_Ch169L').prop('disabled', !project.systemFiles.REG_DOMAIN.ExBO_ENABLED)
        }
        {
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_36 = Math.floor(Number($('#rd_BO_Ch36H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_40 = Math.floor(Number($('#rd_BO_Ch40H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_44 = Math.floor(Number($('#rd_BO_Ch44H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_48 = Math.floor(Number($('#rd_BO_Ch48H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_52 = Math.floor(Number($('#rd_BO_Ch52H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_56 = Math.floor(Number($('#rd_BO_Ch56H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_60 = Math.floor(Number($('#rd_BO_Ch60H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_64 = Math.floor(Number($('#rd_BO_Ch64H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_100 = Math.floor(Number($('#rd_BO_Ch100H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_104 = Math.floor(Number($('#rd_BO_Ch104H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_108 = Math.floor(Number($('#rd_BO_Ch108H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_112 = Math.floor(Number($('#rd_BO_Ch112H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_116 = Math.floor(Number($('#rd_BO_Ch116H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_120 = Math.floor(Number($('#rd_BO_Ch120H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_124 = Math.floor(Number($('#rd_BO_Ch124H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_128 = Math.floor(Number($('#rd_BO_Ch128H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_132 = Math.floor(Number($('#rd_BO_Ch132H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_136 = Math.floor(Number($('#rd_BO_Ch136H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_140 = Math.floor(Number($('#rd_BO_Ch140H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_144 = Math.floor(Number($('#rd_BO_Ch144H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_149 = Math.floor(Number($('#rd_BO_Ch149H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_153 = Math.floor(Number($('#rd_BO_Ch153H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_157 = Math.floor(Number($('#rd_BO_Ch157H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_161 = Math.floor(Number($('#rd_BO_Ch161H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_165 = Math.floor(Number($('#rd_BO_Ch165H').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBH_CHANNEL_169 = Math.floor(Number($('#rd_BO_Ch169H').val()) * 8);

            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_36 = Math.floor(Number($('#rd_BO_Ch36L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_40 = Math.floor(Number($('#rd_BO_Ch40L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_44 = Math.floor(Number($('#rd_BO_Ch44L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_48 = Math.floor(Number($('#rd_BO_Ch48L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_52 = Math.floor(Number($('#rd_BO_Ch52L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_56 = Math.floor(Number($('#rd_BO_Ch56L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_60 = Math.floor(Number($('#rd_BO_Ch60L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_64 = Math.floor(Number($('#rd_BO_Ch64L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_100 = Math.floor(Number($('#rd_BO_Ch100L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_104 = Math.floor(Number($('#rd_BO_Ch104L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_108 = Math.floor(Number($('#rd_BO_Ch108L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_112 = Math.floor(Number($('#rd_BO_Ch112L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_116 = Math.floor(Number($('#rd_BO_Ch116L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_120 = Math.floor(Number($('#rd_BO_Ch120L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_124 = Math.floor(Number($('#rd_BO_Ch124L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_128 = Math.floor(Number($('#rd_BO_Ch128L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_132 = Math.floor(Number($('#rd_BO_Ch132L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_136 = Math.floor(Number($('#rd_BO_Ch136L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_140 = Math.floor(Number($('#rd_BO_Ch140L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_144 = Math.floor(Number($('#rd_BO_Ch144L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_149 = Math.floor(Number($('#rd_BO_Ch149L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_153 = Math.floor(Number($('#rd_BO_Ch153L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_157 = Math.floor(Number($('#rd_BO_Ch157L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_161 = Math.floor(Number($('#rd_BO_Ch161L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_165 = Math.floor(Number($('#rd_BO_Ch165L').val()) * 8);
            project.systemFiles.REG_DOMAIN.EBL_CHANNEL_169 = Math.floor(Number($('#rd_BO_Ch169L').val()) * 8);
        }
        
        saveProjectAPI();
    });

}

function loadIL() {
    //IL default
    {
        $('#rd_IL_TX_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_RX_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_ANTG_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_TX_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_RX_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_ANTG_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_TX_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_RX_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_ANTG_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_TX_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_RX_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_ANTG_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_TX_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_RX_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        $('#rd_IL_ANTG_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
    }
    {
        $('#rd_IL_TX_1'     ).val(project.systemFiles.REG_DOMAIN.UNII1_TX_0     / 8 );
        $('#rd_IL_RX_1'     ).val(project.systemFiles.REG_DOMAIN.UNII1_RX_0     / 8 );
        $('#rd_IL_ANTG_1'   ).val(project.systemFiles.REG_DOMAIN.UNII1_ALG_0    / 8 );
        $('#rd_IL_TX_2A'    ).val(project.systemFiles.REG_DOMAIN.UNII2A_TX_1    / 8 );
        $('#rd_IL_RX_2A'    ).val(project.systemFiles.REG_DOMAIN.UNII2A_RX_1    / 8 );
        $('#rd_IL_ANTG_2A'  ).val(project.systemFiles.REG_DOMAIN.UNII2A_ALG_1   / 8 );
        $('#rd_IL_TX_2C1'   ).val(project.systemFiles.REG_DOMAIN.UNII2C1_TX_2   / 8 );
        $('#rd_IL_RX_2C1'   ).val(project.systemFiles.REG_DOMAIN.UNII2C1_RX_2   / 8 );
        $('#rd_IL_ANTG_2C1' ).val(project.systemFiles.REG_DOMAIN.UNII2C1_ALG_2  / 8 );
        $('#rd_IL_TX_2C2'   ).val(project.systemFiles.REG_DOMAIN.UNII2C2_TX_3   / 8 );
        $('#rd_IL_RX_2C2'   ).val(project.systemFiles.REG_DOMAIN.UNII2C2_RX_3   / 8 );
        $('#rd_IL_ANTG_2C2' ).val(project.systemFiles.REG_DOMAIN.UNII2C2_ALG_3  / 8 );
        $('#rd_IL_TX_3'     ).val(project.systemFiles.REG_DOMAIN.UNII3_TX_4     / 8 );
        $('#rd_IL_RX_3'     ).val(project.systemFiles.REG_DOMAIN.UNII3_RX_4     / 8 );
        $('#rd_IL_ANTG_3'   ).val(project.systemFiles.REG_DOMAIN.UNII3_ALG_4    / 8 );
    }

    $("#regdomcbIL_1").prop("checked", project.systemFiles.REG_DOMAIN.IL_ENABLED);

    $('#regdomcbIL_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.IL_ENABLED = $('#regdomcbIL_1').is(':checked')
        {
            $('#rd_IL_TX_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_RX_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_ANTG_1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_TX_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_RX_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_ANTG_2A').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_TX_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_RX_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_ANTG_2C1').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_TX_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_RX_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_ANTG_2C2').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_TX_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_RX_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
            $('#rd_IL_ANTG_3').prop('disabled', !project.systemFiles.REG_DOMAIN.IL_ENABLED)
        }

        {

            project.systemFiles.REG_DOMAIN.UNII1_TX_0 = Math.floor(Number($('#rd_IL_TX_1').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII1_RX_0 = Math.floor(Number($('#rd_IL_RX_1').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII1_ALG_0 = Math.floor(Number($('#rd_IL_ANTG_1').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2A_TX_1 = Math.floor(Number($('#rd_IL_TX_2A').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2A_RX_1 = Math.floor(Number($('#rd_IL_RX_2A').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2A_ALG_1 = Math.floor(Number($('#rd_IL_ANTG_2A').val()) * 8);
            project.systemFiles.REG_DOMAIN.UNII2C1_TX_2 = Math.floor(Number($('#rd_IL_TX_2C1').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2C1_RX_2 = Math.floor(Number($('#rd_IL_RX_2C1').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2C1_ALG_2 = Math.floor(Number($('#rd_IL_ANTG_2C1').val()) * 8);
            project.systemFiles.REG_DOMAIN.UNII2C2_TX_3 = Math.floor(Number($('#rd_IL_TX_2C2').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2C2_RX_3 = Math.floor(Number($('#rd_IL_RX_2C2').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII2C2_ALG_3 = Math.floor(Number($('#rd_IL_ANTG_2C2').val()) * 8);
            project.systemFiles.REG_DOMAIN.UNII3_TX_4 = Math.floor(Number($('#rd_IL_TX_3').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII3_RX_4 = Math.floor(Number($('#rd_IL_RX_3').val()) * 8); 
            project.systemFiles.REG_DOMAIN.UNII3_ALG_4 = Math.floor(Number($('#rd_IL_ANTG_3').val()) * 8); 
        }
        
        saveProjectAPI();
    });


}

function loadRegulatoryDomain() {

    /*FCC*/
    {
        loadFCC();

        $(document).on('change', '#rd_FCC_Ch36' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_36 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_36 =  Math.floor(Number($('#rd_FCC_Ch36').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch40' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_40 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_40 =  Math.floor(Number($('#rd_FCC_Ch40').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch44' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_44 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_44 =  Math.floor(Number($('#rd_FCC_Ch44').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch48' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_48 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_48 =  Math.floor(Number($('#rd_FCC_Ch48').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch52' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_52 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_52 =  Math.floor(Number($('#rd_FCC_Ch52').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch56' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_56 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_56 =  Math.floor(Number($('#rd_FCC_Ch56').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch60' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_60 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_60 =  Math.floor(Number($('#rd_FCC_Ch60').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch64' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_64 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_64 =  Math.floor(Number($('#rd_FCC_Ch64').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch100', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_100) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_100 = Math.floor(Number($('#rd_FCC_Ch100').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch104', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_104) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_104 = Math.floor(Number($('#rd_FCC_Ch104').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch108', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_108) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_108 = Math.floor(Number($('#rd_FCC_Ch108').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch112', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_112) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_112 = Math.floor(Number($('#rd_FCC_Ch112').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch116', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_116) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_116 = Math.floor(Number($('#rd_FCC_Ch116').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch120', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_120) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_120 = Math.floor(Number($('#rd_FCC_Ch120').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch124', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_124) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_124 = Math.floor(Number($('#rd_FCC_Ch124').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch128', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_128) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_128 = Math.floor(Number($('#rd_FCC_Ch128').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch132', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_132) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_132 = Math.floor(Number($('#rd_FCC_Ch132').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch136', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_136) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_136 = Math.floor(Number($('#rd_FCC_Ch136').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch140', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_140) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_140 = Math.floor(Number($('#rd_FCC_Ch140').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch144', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_144) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_144 = Math.floor(Number($('#rd_FCC_Ch144').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch149', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_149) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_149 = Math.floor(Number($('#rd_FCC_Ch149').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch153', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_153) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_153 = Math.floor(Number($('#rd_FCC_Ch153').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch157', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_157) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_157 = Math.floor(Number($('#rd_FCC_Ch157').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch161', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_161) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_161 = Math.floor(Number($('#rd_FCC_Ch161').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch165', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_165) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_165 = Math.floor(Number($('#rd_FCC_Ch165').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC_Ch169', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.FCC_CHANNEL_169) / 8)) { project.systemFiles.REG_DOMAIN.FCC_CHANNEL_169 = Math.floor(Number($('#rd_FCC_Ch169').val()) * 8); saveProjectAPI(); } });
    }

    /*ETSI*/
    {
        loadETCI();

        $(document).on('change', '#rd_ETCI_Ch36' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_36 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_36  = Math.floor(Number($('#rd_ETCI_Ch36').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch40' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_40 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_40  = Math.floor(Number($('#rd_ETCI_Ch40').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch44' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_44 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_44  = Math.floor(Number($('#rd_ETCI_Ch44').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch48' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_48 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_48  = Math.floor(Number($('#rd_ETCI_Ch48').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch52' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_52 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_52  = Math.floor(Number($('#rd_ETCI_Ch52').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch56' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_56 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_56  = Math.floor(Number($('#rd_ETCI_Ch56').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch60' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_60 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_60  = Math.floor(Number($('#rd_ETCI_Ch60').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch64' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_64 ) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_64  = Math.floor(Number($('#rd_ETCI_Ch64').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch100', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_100) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_100 = Math.floor(Number($('#rd_ETCI_Ch100').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch104', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_104) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_104 = Math.floor(Number($('#rd_ETCI_Ch104').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch108', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_108) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_108 = Math.floor(Number($('#rd_ETCI_Ch108').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch112', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_112) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_112 = Math.floor(Number($('#rd_ETCI_Ch112').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch116', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_116) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_116 = Math.floor(Number($('#rd_ETCI_Ch116').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch120', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_120) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_120 = Math.floor(Number($('#rd_ETCI_Ch120').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch124', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_124) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_124 = Math.floor(Number($('#rd_ETCI_Ch124').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch128', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_128) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_128 = Math.floor(Number($('#rd_ETCI_Ch128').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch132', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_132) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_132 = Math.floor(Number($('#rd_ETCI_Ch132').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch136', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_136) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_136 = Math.floor(Number($('#rd_ETCI_Ch136').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch140', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_140) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_140 = Math.floor(Number($('#rd_ETCI_Ch140').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch144', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_144) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_144 = Math.floor(Number($('#rd_ETCI_Ch144').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch149', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_149) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_149 = Math.floor(Number($('#rd_ETCI_Ch149').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch153', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_153) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_153 = Math.floor(Number($('#rd_ETCI_Ch153').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch157', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_157) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_157 = Math.floor(Number($('#rd_ETCI_Ch157').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch161', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_161) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_161 = Math.floor(Number($('#rd_ETCI_Ch161').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch165', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_165) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_165 = Math.floor(Number($('#rd_ETCI_Ch165').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETCI_Ch169', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_169) / 8)) { project.systemFiles.REG_DOMAIN.ETCI_CHANNEL_169 = Math.floor(Number($('#rd_ETCI_Ch169').val()) * 8); saveProjectAPI(); } });

    }

    /*JP*/
    {
        loadJP();

        $(document).on('change', '#rd_JP_Ch36' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_36 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_36  = Math.floor(Number($('#rd_JP_Ch36' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch40' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_40 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_40  = Math.floor(Number($('#rd_JP_Ch40' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch44' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_44 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_44  = Math.floor(Number($('#rd_JP_Ch44' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch48' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_48 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_48  = Math.floor(Number($('#rd_JP_Ch48' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch52' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_52 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_52  = Math.floor(Number($('#rd_JP_Ch52' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch56' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_56 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_56  = Math.floor(Number($('#rd_JP_Ch56' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch60' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_60 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_60  = Math.floor(Number($('#rd_JP_Ch60' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch64' , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_64 ) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_64  = Math.floor(Number($('#rd_JP_Ch64' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch100', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_100) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_100 = Math.floor(Number($('#rd_JP_Ch100').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch104', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_104) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_104 = Math.floor(Number($('#rd_JP_Ch104').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch108', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_108) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_108 = Math.floor(Number($('#rd_JP_Ch108').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch112', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_112) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_112 = Math.floor(Number($('#rd_JP_Ch112').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch116', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_116) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_116 = Math.floor(Number($('#rd_JP_Ch116').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch120', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_120) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_120 = Math.floor(Number($('#rd_JP_Ch120').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch124', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_124) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_124 = Math.floor(Number($('#rd_JP_Ch124').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch128', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_128) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_128 = Math.floor(Number($('#rd_JP_Ch128').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch132', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_132) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_132 = Math.floor(Number($('#rd_JP_Ch132').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch136', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_136) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_136 = Math.floor(Number($('#rd_JP_Ch136').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch140', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_140) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_140 = Math.floor(Number($('#rd_JP_Ch140').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch144', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_144) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_144 = Math.floor(Number($('#rd_JP_Ch144').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch149', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_149) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_149 = Math.floor(Number($('#rd_JP_Ch149').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch153', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_153) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_153 = Math.floor(Number($('#rd_JP_Ch153').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch157', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_157) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_157 = Math.floor(Number($('#rd_JP_Ch157').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch161', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_161) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_161 = Math.floor(Number($('#rd_JP_Ch161').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch165', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_165) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_165 = Math.floor(Number($('#rd_JP_Ch165').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP_Ch169', function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.JP_CHANNEL_169) / 8)) { project.systemFiles.REG_DOMAIN.JP_CHANNEL_169 = Math.floor(Number($('#rd_JP_Ch169').val()) * 8); saveProjectAPI(); } });
    }

    /*ExBO*/
    {
        loadExtraBO();

        $(document).on('change', '#rd_BO_Ch36H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_36 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_36  = Math.floor(Number($('#rd_BO_Ch36H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch40H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_40 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_40  = Math.floor(Number($('#rd_BO_Ch40H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch44H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_44 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_44  = Math.floor(Number($('#rd_BO_Ch44H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch48H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_48 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_48  = Math.floor(Number($('#rd_BO_Ch48H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch52H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_52 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_52  = Math.floor(Number($('#rd_BO_Ch52H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch56H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_56 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_56  = Math.floor(Number($('#rd_BO_Ch56H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch60H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_60 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_60  = Math.floor(Number($('#rd_BO_Ch60H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch64H' , function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_64 ) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_64  = Math.floor(Number($('#rd_BO_Ch64H' ).val()) * 8); saveProjectAPI();   } });
        $(document).on('change', '#rd_BO_Ch100H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_100) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_100 = Math.floor(Number($('#rd_BO_Ch100H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch104H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_104) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_104 = Math.floor(Number($('#rd_BO_Ch104H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch108H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_108) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_108 = Math.floor(Number($('#rd_BO_Ch108H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch112H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_112) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_112 = Math.floor(Number($('#rd_BO_Ch112H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch116H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_116) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_116 = Math.floor(Number($('#rd_BO_Ch116H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch120H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_120) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_120 = Math.floor(Number($('#rd_BO_Ch120H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch124H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_124) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_124 = Math.floor(Number($('#rd_BO_Ch124H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch128H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_128) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_128 = Math.floor(Number($('#rd_BO_Ch128H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch132H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_132) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_132 = Math.floor(Number($('#rd_BO_Ch132H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch136H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_136) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_136 = Math.floor(Number($('#rd_BO_Ch136H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch140H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_140) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_140 = Math.floor(Number($('#rd_BO_Ch140H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch144H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_144) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_144 = Math.floor(Number($('#rd_BO_Ch144H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch149H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_149) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_149 = Math.floor(Number($('#rd_BO_Ch149H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch153H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_153) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_153 = Math.floor(Number($('#rd_BO_Ch153H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch157H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_157) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_157 = Math.floor(Number($('#rd_BO_Ch157H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch161H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_161) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_161 = Math.floor(Number($('#rd_BO_Ch161H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch165H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_165) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_165 = Math.floor(Number($('#rd_BO_Ch165H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch169H', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBH_CHANNEL_169) / 8)) { project.systemFiles.REG_DOMAIN.EBH_CHANNEL_169 = Math.floor(Number($('#rd_BO_Ch169H').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_BO_Ch36L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_36  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_36  = Math.floor(Number($('#rd_BO_Ch36L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch40L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_40  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_40  = Math.floor(Number($('#rd_BO_Ch40L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch44L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_44  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_44  = Math.floor(Number($('#rd_BO_Ch44L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch48L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_48  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_48  = Math.floor(Number($('#rd_BO_Ch48L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch52L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_52  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_52  = Math.floor(Number($('#rd_BO_Ch52L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch56L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_56  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_56  = Math.floor(Number($('#rd_BO_Ch56L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch60L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_60  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_60  = Math.floor(Number($('#rd_BO_Ch60L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch64L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_64  ) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_64  = Math.floor(Number($('#rd_BO_Ch64L' ).val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch100L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_100) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_100 = Math.floor(Number($('#rd_BO_Ch100L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch104L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_104) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_104 = Math.floor(Number($('#rd_BO_Ch104L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch108L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_108) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_108 = Math.floor(Number($('#rd_BO_Ch108L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch112L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_112) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_112 = Math.floor(Number($('#rd_BO_Ch112L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch116L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_116) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_116 = Math.floor(Number($('#rd_BO_Ch116L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch120L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_120) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_120 = Math.floor(Number($('#rd_BO_Ch120L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch124L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_124) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_124 = Math.floor(Number($('#rd_BO_Ch124L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch128L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_128) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_128 = Math.floor(Number($('#rd_BO_Ch128L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch132L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_132) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_132 = Math.floor(Number($('#rd_BO_Ch132L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch136L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_136) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_136 = Math.floor(Number($('#rd_BO_Ch136L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch140L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_140) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_140 = Math.floor(Number($('#rd_BO_Ch140L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch144L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_144) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_144 = Math.floor(Number($('#rd_BO_Ch144L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch149L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_149) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_149 = Math.floor(Number($('#rd_BO_Ch149L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch153L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_153) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_153 = Math.floor(Number($('#rd_BO_Ch153L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch157L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_157) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_157 = Math.floor(Number($('#rd_BO_Ch157L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch161L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_161) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_161 = Math.floor(Number($('#rd_BO_Ch161L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch165L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_165) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_165 = Math.floor(Number($('#rd_BO_Ch165L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_BO_Ch169L', function (e) { if (verifyNumLimitsTT(e, 0, 6, Number(project.systemFiles.REG_DOMAIN.EBL_CHANNEL_169) / 8)) { project.systemFiles.REG_DOMAIN.EBL_CHANNEL_169 = Math.floor(Number($('#rd_BO_Ch169L').val()) * 8); saveProjectAPI(); } });

    }

    /*IL*/
    {
        loadIL();

        $(document).on('change', '#rd_IL_TX_1'      , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII1_TX_0) / 8)) { project.systemFiles.REG_DOMAIN.UNII1_TX_0        = Math.floor(Number($('#rd_IL_TX_1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_RX_1'      , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII1_RX_0) / 8)) { project.systemFiles.REG_DOMAIN.UNII1_RX_0        = Math.floor(Number($('#rd_IL_RX_1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_ANTG_1'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII1_ALG_0) / 8)) { project.systemFiles.REG_DOMAIN.UNII1_ALG_0      = Math.floor(Number($('#rd_IL_ANTG_1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_TX_2A'     , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2A_TX_1) / 8)) { project.systemFiles.REG_DOMAIN.UNII2A_TX_1      = Math.floor(Number($('#rd_IL_TX_2A').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_RX_2A'     , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2A_RX_1) / 8)) { project.systemFiles.REG_DOMAIN.UNII2A_RX_1      = Math.floor(Number($('#rd_IL_RX_2A').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_ANTG_2A'   , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2A_ALG_1) / 8)) { project.systemFiles.REG_DOMAIN.UNII2A_ALG_1    = Math.floor(Number($('#rd_IL_ANTG_2A').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_TX_2C1'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C1_TX_2) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C1_TX_2    = Math.floor(Number($('#rd_IL_TX_2C1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_RX_2C1'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C1_RX_2) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C1_RX_2    = Math.floor(Number($('#rd_IL_RX_2C1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_ANTG_2C1'  , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C1_ALG_2) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C1_ALG_2  = Math.floor(Number($('#rd_IL_ANTG_2C1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_TX_2C2'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C2_TX_3) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C2_TX_3    = Math.floor(Number($('#rd_IL_TX_2C2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_RX_2C2'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C2_RX_3) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C2_RX_3    = Math.floor(Number($('#rd_IL_RX_2C2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_ANTG_2C2'  , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII2C2_ALG_3) / 8)) { project.systemFiles.REG_DOMAIN.UNII2C2_ALG_3  = Math.floor(Number($('#rd_IL_ANTG_2C2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_TX_3'      , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII3_TX_4) / 8)) { project.systemFiles.REG_DOMAIN.UNII3_TX_4        = Math.floor(Number($('#rd_IL_TX_3').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_RX_3'      , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII3_RX_4) / 8)) { project.systemFiles.REG_DOMAIN.UNII3_RX_4        = Math.floor(Number($('#rd_IL_RX_3').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_IL_ANTG_3'    , function (e) { if (verifyNumLimitsTT(e, 0, 32, Number(project.systemFiles.REG_DOMAIN.UNII3_ALG_4) / 8)) { project.systemFiles.REG_DOMAIN.UNII3_ALG_4      = Math.floor(Number($('#rd_IL_ANTG_3').val()) * 8); saveProjectAPI(); } });
    }
}
