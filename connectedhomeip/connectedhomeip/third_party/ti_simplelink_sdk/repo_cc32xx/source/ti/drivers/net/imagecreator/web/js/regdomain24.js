
function loadFCC24()
{
    {
        $('#rd_FCC11B_Ch1' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch2' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch3' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch4' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch5' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch6' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch7' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch8' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch9' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        /*
        $('#rd_FCC11B_Ch12').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch13').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCC11B_Ch14').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        */
        $('#rd_FCC11B_Ch12').prop('disabled', true)
        $('#rd_FCC11B_Ch13').prop('disabled', true)
        $('#rd_FCC11B_Ch14').prop('disabled', true)

        $('#rd_FCCBO_Ch1H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch2H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch3H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch4H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch5H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch6H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch7H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch8H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch9H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        /*
        $('#rd_FCCBO_Ch12H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch13H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch14H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        */
        $('#rd_FCCBO_Ch12H').prop('disabled', true)
        $('#rd_FCCBO_Ch13H').prop('disabled', true)
        $('#rd_FCCBO_Ch14H').prop('disabled', true)

        $('#rd_FCCBO_Ch1L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch2L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch3L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch4L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch5L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch6L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch7L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch8L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch9L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        /*
        $('#rd_FCCBO_Ch12L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch13L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        $('#rd_FCCBO_Ch14L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
        */
        $('#rd_FCCBO_Ch12L').prop('disabled', true)
        $('#rd_FCCBO_Ch13L').prop('disabled', true)
        $('#rd_FCCBO_Ch14L').prop('disabled', true)

    }
    {
        $('#rd_FCC11B_Ch1' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_1  / 8);
        $('#rd_FCC11B_Ch2' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_2  / 8);
        $('#rd_FCC11B_Ch3' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_3  / 8);
        $('#rd_FCC11B_Ch4' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_4  / 8);
        $('#rd_FCC11B_Ch5' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_5  / 8);
        $('#rd_FCC11B_Ch6' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_6  / 8);
        $('#rd_FCC11B_Ch7' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_7  / 8);
        $('#rd_FCC11B_Ch8' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_8  / 8);
        $('#rd_FCC11B_Ch9' ).val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_9  / 8);
        $('#rd_FCC11B_Ch10').val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_10 / 8);
        $('#rd_FCC11B_Ch11').val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_11 / 8);
        $('#rd_FCC11B_Ch12').val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_12 / 8);
        $('#rd_FCC11B_Ch13').val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_13 / 8);
        $('#rd_FCC11B_Ch14').val(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_14 / 8);

        $('#rd_FCCBO_Ch1H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_1 / 8);
        $('#rd_FCCBO_Ch2H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_2 / 8);
        $('#rd_FCCBO_Ch3H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_3 / 8);
        $('#rd_FCCBO_Ch4H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_4 / 8);
        $('#rd_FCCBO_Ch5H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_5 / 8);
        $('#rd_FCCBO_Ch6H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_6 / 8);
        $('#rd_FCCBO_Ch7H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_7 / 8);
        $('#rd_FCCBO_Ch8H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_8 / 8);
        $('#rd_FCCBO_Ch9H' ).val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_9 / 8);
        $('#rd_FCCBO_Ch10H').val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_10 / 8);
        $('#rd_FCCBO_Ch11H').val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_11 / 8);
        $('#rd_FCCBO_Ch12H').val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_12 / 8);
        $('#rd_FCCBO_Ch13H').val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_13 / 8);
        $('#rd_FCCBO_Ch14H').val(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_14 / 8);

        $('#rd_FCCBO_Ch1L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_1 / 8);
        $('#rd_FCCBO_Ch2L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_2 / 8);
        $('#rd_FCCBO_Ch3L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_3 / 8);
        $('#rd_FCCBO_Ch4L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_4 / 8);
        $('#rd_FCCBO_Ch5L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_5 / 8);
        $('#rd_FCCBO_Ch6L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_6 / 8);
        $('#rd_FCCBO_Ch7L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_7 / 8);
        $('#rd_FCCBO_Ch8L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_8 / 8);
        $('#rd_FCCBO_Ch9L' ).val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_9 / 8);
        $('#rd_FCCBO_Ch10L').val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_10 / 8);
        $('#rd_FCCBO_Ch11L').val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_11 / 8);
        $('#rd_FCCBO_Ch12L').val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_12 / 8);
        $('#rd_FCCBO_Ch13L').val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_13 / 8);
        $('#rd_FCCBO_Ch14L').val(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_14 / 8);
        
    }
    $("#regdom24cbFCCBO_1").prop("checked", project.systemFiles.REG_DOMAIN.FCC24_ENABLED);

    $('#regdom24cbFCCBO_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.FCC24_ENABLED = $('#regdom24cbFCCBO_1').is(':checked')
        {
            $('#rd_FCC11B_Ch1' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch2' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch3' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch4' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch5' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch6' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch7' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch8' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch9' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCC11B_Ch12').prop('disabled', true)
            $('#rd_FCC11B_Ch13').prop('disabled', true)
            $('#rd_FCC11B_Ch14').prop('disabled', true)
           
            $('#rd_FCCBO_Ch1H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch2H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch3H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch4H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch5H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch6H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch7H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch8H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch9H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch12H').prop('disabled', true)
            $('#rd_FCCBO_Ch13H').prop('disabled', true)
            $('#rd_FCCBO_Ch14H').prop('disabled', true)

            $('#rd_FCCBO_Ch1L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch2L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch3L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch4L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch5L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch6L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch7L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch8L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch9L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.FCC24_ENABLED)
            $('#rd_FCCBO_Ch12L').prop('disabled', true)
            $('#rd_FCCBO_Ch13L').prop('disabled', true)
            $('#rd_FCCBO_Ch14L').prop('disabled', true)
        }
        {
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_1 = Math.floor(Number($('#rd_FCC11B_Ch1').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_2 = Math.floor(Number($('#rd_FCC11B_Ch2').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_3 = Math.floor(Number($('#rd_FCC11B_Ch3').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_4 = Math.floor(Number($('#rd_FCC11B_Ch4').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_5 = Math.floor(Number($('#rd_FCC11B_Ch5').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_6 = Math.floor(Number($('#rd_FCC11B_Ch6').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_7 = Math.floor(Number($('#rd_FCC11B_Ch7').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_8 = Math.floor(Number($('#rd_FCC11B_Ch8').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_9 = Math.floor(Number($('#rd_FCC11B_Ch9').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_10 = Math.floor(Number($('#rd_FCC11B_Ch10').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_11 = Math.floor(Number($('#rd_FCC11B_Ch11').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_12 = Math.floor(Number($('#rd_FCC11B_Ch12').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_13 = Math.floor(Number($('#rd_FCC11B_Ch13').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_14 = Math.floor(Number($('#rd_FCC11B_Ch14').val()) * 8);
            
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_1 = Math.floor(Number($('#rd_FCCBO_Ch1H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_2 = Math.floor(Number($('#rd_FCCBO_Ch2H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_3 = Math.floor(Number($('#rd_FCCBO_Ch3H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_4 = Math.floor(Number($('#rd_FCCBO_Ch4H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_5 = Math.floor(Number($('#rd_FCCBO_Ch5H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_6 = Math.floor(Number($('#rd_FCCBO_Ch6H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_7 = Math.floor(Number($('#rd_FCCBO_Ch7H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_8 = Math.floor(Number($('#rd_FCCBO_Ch8H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_9 = Math.floor(Number($('#rd_FCCBO_Ch9H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_10 = Math.floor(Number($('#rd_FCCBO_Ch10H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_11 = Math.floor(Number($('#rd_FCCBO_Ch11H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_12 = Math.floor(Number($('#rd_FCCBO_Ch12H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_13 = Math.floor(Number($('#rd_FCCBO_Ch13H').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_14 = Math.floor(Number($('#rd_FCCBO_Ch14H').val()) * 8);

            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_1 = Math.floor(Number($('#rd_FCCBO_Ch1L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_2 = Math.floor(Number($('#rd_FCCBO_Ch2L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_3 = Math.floor(Number($('#rd_FCCBO_Ch3L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_4 = Math.floor(Number($('#rd_FCCBO_Ch4L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_5 = Math.floor(Number($('#rd_FCCBO_Ch5L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_6 = Math.floor(Number($('#rd_FCCBO_Ch6L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_7 = Math.floor(Number($('#rd_FCCBO_Ch7L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_8 = Math.floor(Number($('#rd_FCCBO_Ch8L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_9 = Math.floor(Number($('#rd_FCCBO_Ch9L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_10 = Math.floor(Number($('#rd_FCCBO_Ch10L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_11 = Math.floor(Number($('#rd_FCCBO_Ch11L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_12 = Math.floor(Number($('#rd_FCCBO_Ch12L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_13 = Math.floor(Number($('#rd_FCCBO_Ch13L').val()) * 8);
            project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_14 = Math.floor(Number($('#rd_FCCBO_Ch14L').val()) * 8);
        }
        saveProjectAPI();

    });
}

function loadETSI24() {
    {
        $('#rd_ETSI11B_Ch1').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch2').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch3').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch4').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch5').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch6').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch7').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch8').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch9').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch12').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch13').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSI11B_Ch14').prop('disabled', true)

        $('#rd_ETSIBO_Ch1H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch2H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch3H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch4H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch5H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch6H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch7H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch8H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch9H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch12H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch13H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch14H').prop('disabled', true)

        $('#rd_ETSIBO_Ch1L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch2L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch3L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch4L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch5L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch6L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch7L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch8L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch9L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch12L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch13L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
        $('#rd_ETSIBO_Ch14L').prop('disabled', true)
    }
    {
        $('#rd_ETSI11B_Ch1' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_1 / 8);
        $('#rd_ETSI11B_Ch2' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_2 / 8);
        $('#rd_ETSI11B_Ch3' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_3 / 8);
        $('#rd_ETSI11B_Ch4' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_4 / 8);
        $('#rd_ETSI11B_Ch5' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_5 / 8);
        $('#rd_ETSI11B_Ch6' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_6 / 8);
        $('#rd_ETSI11B_Ch7' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_7 / 8);
        $('#rd_ETSI11B_Ch8' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_8 / 8);
        $('#rd_ETSI11B_Ch9' ).val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_9 / 8);
        $('#rd_ETSI11B_Ch10').val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_10 / 8);
        $('#rd_ETSI11B_Ch11').val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_11 / 8);
        $('#rd_ETSI11B_Ch12').val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_12 / 8);
        $('#rd_ETSI11B_Ch13').val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_13 / 8);
        $('#rd_ETSI11B_Ch14').val(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_14 / 8);

        $('#rd_ETSIBO_Ch1H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_1 / 8);
        $('#rd_ETSIBO_Ch2H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_2 / 8);
        $('#rd_ETSIBO_Ch3H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_3 / 8);
        $('#rd_ETSIBO_Ch4H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_4 / 8);
        $('#rd_ETSIBO_Ch5H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_5 / 8);
        $('#rd_ETSIBO_Ch6H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_6 / 8);
        $('#rd_ETSIBO_Ch7H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_7 / 8);
        $('#rd_ETSIBO_Ch8H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_8 / 8);
        $('#rd_ETSIBO_Ch9H' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_9 / 8);
        $('#rd_ETSIBO_Ch10H').val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_10 / 8);
        $('#rd_ETSIBO_Ch11H').val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_11 / 8);
        $('#rd_ETSIBO_Ch12H').val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_12 / 8);
        $('#rd_ETSIBO_Ch13H').val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_13 / 8);
        $('#rd_ETSIBO_Ch14H').val(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_14 / 8);

        $('#rd_ETSIBO_Ch1L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_1 / 8);
        $('#rd_ETSIBO_Ch2L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_2 / 8);
        $('#rd_ETSIBO_Ch3L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_3 / 8);
        $('#rd_ETSIBO_Ch4L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_4 / 8);
        $('#rd_ETSIBO_Ch5L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_5 / 8);
        $('#rd_ETSIBO_Ch6L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_6 / 8);
        $('#rd_ETSIBO_Ch7L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_7 / 8);
        $('#rd_ETSIBO_Ch8L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_8 / 8);
        $('#rd_ETSIBO_Ch9L' ).val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_9 / 8);
        $('#rd_ETSIBO_Ch10L').val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_10 / 8);
        $('#rd_ETSIBO_Ch11L').val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_11 / 8);
        $('#rd_ETSIBO_Ch12L').val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_12 / 8);
        $('#rd_ETSIBO_Ch13L').val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_13 / 8);
        $('#rd_ETSIBO_Ch14L').val(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_14 / 8);

    }
    $("#regdom24cbETSIBO_1").prop("checked", project.systemFiles.REG_DOMAIN.ETSI24_ENABLED);

    $('#regdom24cbETSIBO_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.ETSI24_ENABLED = $('#regdom24cbETSIBO_1').is(':checked')
        {
            $('#rd_ETSI11B_Ch1').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch2').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch3').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch4').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch5').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch6').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch7').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch8').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch9').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch12').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch13').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSI11B_Ch14').prop('disabled', true)

            $('#rd_ETSIBO_Ch1H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch2H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch3H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch4H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch5H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch6H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch7H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch8H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch9H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch12H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch13H').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch14H').prop('disabled', true)

            $('#rd_ETSIBO_Ch1L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch2L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch3L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch4L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch5L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch6L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch7L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch8L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch9L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch12L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch13L').prop('disabled', !project.systemFiles.REG_DOMAIN.ETSI24_ENABLED)
            $('#rd_ETSIBO_Ch14L').prop('disabled', true)
        }
        {
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_1  = Math.floor(Number($('#rd_ETSI11B_Ch1').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_2  = Math.floor(Number($('#rd_ETSI11B_Ch2').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_3  = Math.floor(Number($('#rd_ETSI11B_Ch3').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_4  = Math.floor(Number($('#rd_ETSI11B_Ch4').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_5  = Math.floor(Number($('#rd_ETSI11B_Ch5').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_6  = Math.floor(Number($('#rd_ETSI11B_Ch6').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_7  = Math.floor(Number($('#rd_ETSI11B_Ch7').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_8  = Math.floor(Number($('#rd_ETSI11B_Ch8').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_9  = Math.floor(Number($('#rd_ETSI11B_Ch9').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_10 = Math.floor(Number($('#rd_ETSI11B_Ch10').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_11 = Math.floor(Number($('#rd_ETSI11B_Ch11').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_12 = Math.floor(Number($('#rd_ETSI11B_Ch12').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_13 = Math.floor(Number($('#rd_ETSI11B_Ch13').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_14 = Math.floor(Number($('#rd_ETSI11B_Ch14').val()) * 8);

            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_1  = Math.floor(Number($('#rd_ETSIBO_Ch1H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_2  = Math.floor(Number($('#rd_ETSIBO_Ch2H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_3  = Math.floor(Number($('#rd_ETSIBO_Ch3H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_4  = Math.floor(Number($('#rd_ETSIBO_Ch4H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_5  = Math.floor(Number($('#rd_ETSIBO_Ch5H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_6  = Math.floor(Number($('#rd_ETSIBO_Ch6H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_7  = Math.floor(Number($('#rd_ETSIBO_Ch7H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_8  = Math.floor(Number($('#rd_ETSIBO_Ch8H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_9  = Math.floor(Number($('#rd_ETSIBO_Ch9H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_10 = Math.floor(Number($('#rd_ETSIBO_Ch10H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_11 = Math.floor(Number($('#rd_ETSIBO_Ch11H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_12 = Math.floor(Number($('#rd_ETSIBO_Ch12H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_13 = Math.floor(Number($('#rd_ETSIBO_Ch13H').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_14 = Math.floor(Number($('#rd_ETSIBO_Ch14H').val()) * 8);

            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_1  = Math.floor(Number($('#rd_ETSIBO_Ch1L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_2  = Math.floor(Number($('#rd_ETSIBO_Ch2L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_3  = Math.floor(Number($('#rd_ETSIBO_Ch3L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_4  = Math.floor(Number($('#rd_ETSIBO_Ch4L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_5  = Math.floor(Number($('#rd_ETSIBO_Ch5L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_6  = Math.floor(Number($('#rd_ETSIBO_Ch6L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_7  = Math.floor(Number($('#rd_ETSIBO_Ch7L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_8  = Math.floor(Number($('#rd_ETSIBO_Ch8L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_9  = Math.floor(Number($('#rd_ETSIBO_Ch9L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_10 = Math.floor(Number($('#rd_ETSIBO_Ch10L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_11 = Math.floor(Number($('#rd_ETSIBO_Ch11L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_12 = Math.floor(Number($('#rd_ETSIBO_Ch12L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_13 = Math.floor(Number($('#rd_ETSIBO_Ch13L').val()) * 8);
            project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_14 = Math.floor(Number($('#rd_ETSIBO_Ch14L').val()) * 8);
        }
        saveProjectAPI();

    });
}

function loadJP24() {
    {
        $('#rd_JP11B_Ch1' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch2' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch3' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch4' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch5' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch6' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch7' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch8' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch9' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch12').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch13').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JP11B_Ch14').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)

        $('#rd_JPBO_Ch1H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch2H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch3H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch4H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch5H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch6H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch7H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch8H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch9H' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch12H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch13H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch14H').prop('disabled', true)

        $('#rd_JPBO_Ch1L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch2L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch3L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch4L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch5L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch6L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch7L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch8L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch9L' ).prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch12L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch13L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
        $('#rd_JPBO_Ch14L').prop('disabled', true)
    }
    {
        $('#rd_JP11B_Ch1').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_1 / 8);
        $('#rd_JP11B_Ch2').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_2 / 8);
        $('#rd_JP11B_Ch3').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_3 / 8);
        $('#rd_JP11B_Ch4').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_4 / 8);
        $('#rd_JP11B_Ch5').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_5 / 8);
        $('#rd_JP11B_Ch6').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_6 / 8);
        $('#rd_JP11B_Ch7').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_7 / 8);
        $('#rd_JP11B_Ch8').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_8 / 8);
        $('#rd_JP11B_Ch9').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_9 / 8);
        $('#rd_JP11B_Ch10').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_10 / 8);
        $('#rd_JP11B_Ch11').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_11 / 8);
        $('#rd_JP11B_Ch12').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_12 / 8);
        $('#rd_JP11B_Ch13').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_13 / 8);
        $('#rd_JP11B_Ch14').val(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_14 / 8);

        $('#rd_JPBO_Ch1H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_1 / 8);
        $('#rd_JPBO_Ch2H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_2 / 8);
        $('#rd_JPBO_Ch3H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_3 / 8);
        $('#rd_JPBO_Ch4H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_4 / 8);
        $('#rd_JPBO_Ch5H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_5 / 8);
        $('#rd_JPBO_Ch6H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_6 / 8);
        $('#rd_JPBO_Ch7H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_7 / 8);
        $('#rd_JPBO_Ch8H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_8 / 8);
        $('#rd_JPBO_Ch9H' ).val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_9 / 8);
        $('#rd_JPBO_Ch10H').val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_10 / 8);
        $('#rd_JPBO_Ch11H').val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_11 / 8);
        $('#rd_JPBO_Ch12H').val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_12 / 8);
        $('#rd_JPBO_Ch13H').val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_13 / 8);
        $('#rd_JPBO_Ch14H').val(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_14 / 8);

        $('#rd_JPBO_Ch1L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_1 / 8);
        $('#rd_JPBO_Ch2L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_2 / 8);
        $('#rd_JPBO_Ch3L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_3 / 8);
        $('#rd_JPBO_Ch4L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_4 / 8);
        $('#rd_JPBO_Ch5L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_5 / 8);
        $('#rd_JPBO_Ch6L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_6 / 8);
        $('#rd_JPBO_Ch7L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_7 / 8);
        $('#rd_JPBO_Ch8L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_8 / 8);
        $('#rd_JPBO_Ch9L' ).val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_9 / 8);
        $('#rd_JPBO_Ch10L').val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_10 / 8);
        $('#rd_JPBO_Ch11L').val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_11 / 8);
        $('#rd_JPBO_Ch12L').val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_12 / 8);
        $('#rd_JPBO_Ch13L').val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_13 / 8);
        $('#rd_JPBO_Ch14L').val(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_14 / 8);

    }
    $("#regdom24cbJPBO_1").prop("checked", project.systemFiles.REG_DOMAIN.JP24_ENABLED);

    $('#regdom24cbJPBO_1').change(function (e) {

        project.systemFiles.REG_DOMAIN.JP24_ENABLED = $('#regdom24cbJPBO_1').is(':checked')
        {
            $('#rd_JP11B_Ch1').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch2').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch3').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch4').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch5').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch6').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch7').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch8').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch9').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch10').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch11').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch12').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch13').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JP11B_Ch14').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)

            $('#rd_JPBO_Ch1H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch2H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch3H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch4H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch5H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch6H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch7H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch8H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch9H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch10H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch11H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch12H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch13H').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch14H').prop('disabled', true)

            $('#rd_JPBO_Ch1L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch2L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch3L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch4L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch5L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch6L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch7L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch8L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch9L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch10L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch11L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch12L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch13L').prop('disabled', !project.systemFiles.REG_DOMAIN.JP24_ENABLED)
            $('#rd_JPBO_Ch14L').prop('disabled', true)
        }
        {
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_1  = Math.floor(Number($('#rd_JP11B_Ch1').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_2  = Math.floor(Number($('#rd_JP11B_Ch2').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_3  = Math.floor(Number($('#rd_JP11B_Ch3').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_4  = Math.floor(Number($('#rd_JP11B_Ch4').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_5  = Math.floor(Number($('#rd_JP11B_Ch5').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_6  = Math.floor(Number($('#rd_JP11B_Ch6').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_7  = Math.floor(Number($('#rd_JP11B_Ch7').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_8  = Math.floor(Number($('#rd_JP11B_Ch8').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_9  = Math.floor(Number($('#rd_JP11B_Ch9').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_10 = Math.floor(Number($('#rd_JP11B_Ch10').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_11 = Math.floor(Number($('#rd_JP11B_Ch11').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_12 = Math.floor(Number($('#rd_JP11B_Ch12').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_13 = Math.floor(Number($('#rd_JP11B_Ch13').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_14 = Math.floor(Number($('#rd_JP11B_Ch14').val()) * 8);

            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_1  = Math.floor(Number($('#rd_JPBO_Ch1H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_2  = Math.floor(Number($('#rd_JPBO_Ch2H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_3  = Math.floor(Number($('#rd_JPBO_Ch3H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_4  = Math.floor(Number($('#rd_JPBO_Ch4H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_5  = Math.floor(Number($('#rd_JPBO_Ch5H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_6  = Math.floor(Number($('#rd_JPBO_Ch6H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_7  = Math.floor(Number($('#rd_JPBO_Ch7H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_8  = Math.floor(Number($('#rd_JPBO_Ch8H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_9  = Math.floor(Number($('#rd_JPBO_Ch9H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_10 = Math.floor(Number($('#rd_JPBO_Ch10H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_11 = Math.floor(Number($('#rd_JPBO_Ch11H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_12 = Math.floor(Number($('#rd_JPBO_Ch12H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_13 = Math.floor(Number($('#rd_JPBO_Ch13H').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP4BOH_CHANNEL_14 = Math.floor(Number($('#rd_JPBO_Ch14H').val()) * 8);

            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_1  = Math.floor(Number($('#rd_JPBO_Ch1L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_2  = Math.floor(Number($('#rd_JPBO_Ch2L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_3  = Math.floor(Number($('#rd_JPBO_Ch3L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_4  = Math.floor(Number($('#rd_JPBO_Ch4L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_5  = Math.floor(Number($('#rd_JPBO_Ch5L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_6  = Math.floor(Number($('#rd_JPBO_Ch6L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_7  = Math.floor(Number($('#rd_JPBO_Ch7L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_8  = Math.floor(Number($('#rd_JPBO_Ch8L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_9  = Math.floor(Number($('#rd_JPBO_Ch9L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_10 = Math.floor(Number($('#rd_JPBO_Ch10L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_11 = Math.floor(Number($('#rd_JPBO_Ch11L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_12 = Math.floor(Number($('#rd_JPBO_Ch12L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_13 = Math.floor(Number($('#rd_JPBO_Ch13L').val()) * 8);
            project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_14 = Math.floor(Number($('#rd_JPBO_Ch14L').val()) * 8);
        }
        saveProjectAPI();

    });
}

function loadRegulatoryDomain24() {

    /*FCC24*/
    {
        loadFCC24();

        $(document).on('change', '#rd_FCC11B_Ch1', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_1 = Math.floor(Number($('#rd_FCC11B_Ch1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch2', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_2 = Math.floor(Number($('#rd_FCC11B_Ch2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch3', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_3 = Math.floor(Number($('#rd_FCC11B_Ch3').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch4', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_4 = Math.floor(Number($('#rd_FCC11B_Ch4').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch5', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_5 = Math.floor(Number($('#rd_FCC11B_Ch5').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch6', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_6 = Math.floor(Number($('#rd_FCC11B_Ch6').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch7', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_7 = Math.floor(Number($('#rd_FCC11B_Ch7').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch8', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_8 = Math.floor(Number($('#rd_FCC11B_Ch8').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch9', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_9 = Math.floor(Number($('#rd_FCC11B_Ch9').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch10', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_10 = Math.floor(Number($('#rd_FCC11B_Ch10').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch11', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_11 = Math.floor(Number($('#rd_FCC11B_Ch11').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch12', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_12 = Math.floor(Number($('#rd_FCC11B_Ch12').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCC11B_Ch13', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_13 = Math.floor(Number($('#rd_FCC11B_Ch13').val()) * 8); saveProjectAPI(); } });
        //$(document).on('change', '#rd_FCC11B_Ch14', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.FCC2411B_CHANNEL_14 = Math.floor(Number($('#rd_FCC11B_Ch14').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_FCCBO_Ch1H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_1 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_1 = Math.floor(Number($('#rd_FCCBO_Ch1H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch2H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_2 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_2 = Math.floor(Number($('#rd_FCCBO_Ch2H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch3H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_3 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_3 = Math.floor(Number($('#rd_FCCBO_Ch3H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch4H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_4 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_4 = Math.floor(Number($('#rd_FCCBO_Ch4H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch5H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_5 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_5 = Math.floor(Number($('#rd_FCCBO_Ch5H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch6H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_6 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_6 = Math.floor(Number($('#rd_FCCBO_Ch6H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch7H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_7 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_7 = Math.floor(Number($('#rd_FCCBO_Ch7H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch8H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_8 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_8 = Math.floor(Number($('#rd_FCCBO_Ch8H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch9H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_9 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_9 = Math.floor(Number($('#rd_FCCBO_Ch9H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch10H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_10 = Math.floor(Number($('#rd_FCCBO_Ch10H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch11H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_11 = Math.floor(Number($('#rd_FCCBO_Ch11H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch12H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_12 = Math.floor(Number($('#rd_FCCBO_Ch12H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch13H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_13 = Math.floor(Number($('#rd_FCCBO_Ch13H').val()) * 8); saveProjectAPI(); } });
        //$(document).on('change', '#rd_FCCBO_Ch14H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOH_CHANNEL_14 = Math.floor(Number($('#rd_FCCBO_Ch14H').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_FCCBO_Ch1L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_1 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_1 = Math.floor(Number($('#rd_FCCBO_Ch1L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch2L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_2 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_2 = Math.floor(Number($('#rd_FCCBO_Ch2L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch3L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_3 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_3 = Math.floor(Number($('#rd_FCCBO_Ch3L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch4L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_4 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_4 = Math.floor(Number($('#rd_FCCBO_Ch4L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch5L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_5 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_5 = Math.floor(Number($('#rd_FCCBO_Ch5L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch6L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_6 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_6 = Math.floor(Number($('#rd_FCCBO_Ch6L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch7L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_7 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_7 = Math.floor(Number($('#rd_FCCBO_Ch7L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch8L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_8 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_8 = Math.floor(Number($('#rd_FCCBO_Ch8L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch9L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_9 ) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_9 = Math.floor(Number($('#rd_FCCBO_Ch9L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch10L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_10 = Math.floor(Number($('#rd_FCCBO_Ch10L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch11L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_11 = Math.floor(Number($('#rd_FCCBO_Ch11L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch12L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_12 = Math.floor(Number($('#rd_FCCBO_Ch12L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_FCCBO_Ch13L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_13 = Math.floor(Number($('#rd_FCCBO_Ch13L').val()) * 8); saveProjectAPI(); } });
        //$(document).on('change', '#rd_FCCBO_Ch14L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.FCC24BOL_CHANNEL_14 = Math.floor(Number($('#rd_FCCBO_Ch14L').val()) * 8); saveProjectAPI(); } }); 
    }

    /*ETSI24*/
    {
        loadETSI24();

        $(document).on('change', '#rd_ETSI11B_Ch1', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_1 = Math.floor(Number($('#rd_ETSI11B_Ch1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch2', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_2 = Math.floor(Number($('#rd_ETSI11B_Ch2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch3', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_3 = Math.floor(Number($('#rd_ETSI11B_Ch3').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch4', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_4 = Math.floor(Number($('#rd_ETSI11B_Ch4').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch5', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_5 = Math.floor(Number($('#rd_ETSI11B_Ch5').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch6', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_6 = Math.floor(Number($('#rd_ETSI11B_Ch6').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch7', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_7 = Math.floor(Number($('#rd_ETSI11B_Ch7').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch8', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_8 = Math.floor(Number($('#rd_ETSI11B_Ch8').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch9', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_9 = Math.floor(Number($('#rd_ETSI11B_Ch9').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch10', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_10 = Math.floor(Number($('#rd_ETSI11B_Ch10').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch11', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_11 = Math.floor(Number($('#rd_ETSI11B_Ch11').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch12', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_12 = Math.floor(Number($('#rd_ETSI11B_Ch12').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch13', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_13 = Math.floor(Number($('#rd_ETSI11B_Ch13').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSI11B_Ch14', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.ETSI2411B_CHANNEL_14 = Math.floor(Number($('#rd_ETSI11B_Ch14').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_ETSIBO_Ch1H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_1 = Math.floor(Number($('#rd_ETSIBO_Ch1H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch2H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_2 = Math.floor(Number($('#rd_ETSIBO_Ch2H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch3H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_3 = Math.floor(Number($('#rd_ETSIBO_Ch3H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch4H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_4 = Math.floor(Number($('#rd_ETSIBO_Ch4H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch5H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_5 = Math.floor(Number($('#rd_ETSIBO_Ch5H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch6H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_6 = Math.floor(Number($('#rd_ETSIBO_Ch6H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch7H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_7 = Math.floor(Number($('#rd_ETSIBO_Ch7H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch8H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_8 = Math.floor(Number($('#rd_ETSIBO_Ch8H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch9H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_9 = Math.floor(Number($('#rd_ETSIBO_Ch9H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch10H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_10 = Math.floor(Number($('#rd_ETSIBO_Ch10H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch11H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_11 = Math.floor(Number($('#rd_ETSIBO_Ch11H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch12H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_12 = Math.floor(Number($('#rd_ETSIBO_Ch12H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch13H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_13 = Math.floor(Number($('#rd_ETSIBO_Ch13H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch14H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOH_CHANNEL_14 = Math.floor(Number($('#rd_ETSIBO_Ch14H').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_ETSIBO_Ch1L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_1 = Math.floor(Number($('#rd_ETSIBO_Ch1L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch2L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_2 = Math.floor(Number($('#rd_ETSIBO_Ch2L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch3L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_3 = Math.floor(Number($('#rd_ETSIBO_Ch3L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch4L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_4 = Math.floor(Number($('#rd_ETSIBO_Ch4L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch5L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_5 = Math.floor(Number($('#rd_ETSIBO_Ch5L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch6L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_6 = Math.floor(Number($('#rd_ETSIBO_Ch6L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch7L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_7 = Math.floor(Number($('#rd_ETSIBO_Ch7L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch8L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_8 = Math.floor(Number($('#rd_ETSIBO_Ch8L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch9L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_9 = Math.floor(Number($('#rd_ETSIBO_Ch9L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch10L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_10 = Math.floor(Number($('#rd_ETSIBO_Ch10L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch11L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_11 = Math.floor(Number($('#rd_ETSIBO_Ch11L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch12L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_12 = Math.floor(Number($('#rd_ETSIBO_Ch12L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch13L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_13 = Math.floor(Number($('#rd_ETSIBO_Ch13L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_ETSIBO_Ch14L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.ETSI24BOL_CHANNEL_14 = Math.floor(Number($('#rd_ETSIBO_Ch14L').val()) * 8); saveProjectAPI(); } });
    }

    /*JP24*/
    {
        loadJP24();

        $(document).on('change', '#rd_JP11B_Ch1', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_1 = Math.floor(Number($('#rd_JP11B_Ch1').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch2', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_2 = Math.floor(Number($('#rd_JP11B_Ch2').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch3', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_3 = Math.floor(Number($('#rd_JP11B_Ch3').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch4', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_4 = Math.floor(Number($('#rd_JP11B_Ch4').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch5', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_5 = Math.floor(Number($('#rd_JP11B_Ch5').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch6', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_6 = Math.floor(Number($('#rd_JP11B_Ch6').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch7', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_7 = Math.floor(Number($('#rd_JP11B_Ch7').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch8', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_8 = Math.floor(Number($('#rd_JP11B_Ch8').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch9', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_9 = Math.floor(Number($('#rd_JP11B_Ch9').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch10', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_10 = Math.floor(Number($('#rd_JP11B_Ch10').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch11', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_11 = Math.floor(Number($('#rd_JP11B_Ch11').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch12', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_12 = Math.floor(Number($('#rd_JP11B_Ch12').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch13', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_13 = Math.floor(Number($('#rd_JP11B_Ch13').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JP11B_Ch14', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.JP2411B_CHANNEL_14 = Math.floor(Number($('#rd_JP11B_Ch14').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_JPBO_Ch1H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_1 = Math.floor(Number($('#rd_JPBO_Ch1H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch2H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_2 = Math.floor(Number($('#rd_JPBO_Ch2H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch3H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_3 = Math.floor(Number($('#rd_JPBO_Ch3H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch4H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_4 = Math.floor(Number($('#rd_JPBO_Ch4H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch5H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_5 = Math.floor(Number($('#rd_JPBO_Ch5H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch6H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_6 = Math.floor(Number($('#rd_JPBO_Ch6H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch7H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_7 = Math.floor(Number($('#rd_JPBO_Ch7H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch8H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_8 = Math.floor(Number($('#rd_JPBO_Ch8H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch9H' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_9 = Math.floor(Number($('#rd_JPBO_Ch9H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch10H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_10 = Math.floor(Number($('#rd_JPBO_Ch10H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch11H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_11 = Math.floor(Number($('#rd_JPBO_Ch11H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch12H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_12 = Math.floor(Number($('#rd_JPBO_Ch12H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch13H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_13 = Math.floor(Number($('#rd_JPBO_Ch13H').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch14H', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOH_CHANNEL_14 = Math.floor(Number($('#rd_JPBO_Ch14H').val()) * 8); saveProjectAPI(); } });

        $(document).on('change', '#rd_JPBO_Ch1L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_1) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_1 = Math.floor(Number($('#rd_JPBO_Ch1L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch2L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_2) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_2 = Math.floor(Number($('#rd_JPBO_Ch2L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch3L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_3) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_3 = Math.floor(Number($('#rd_JPBO_Ch3L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch4L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_4) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_4 = Math.floor(Number($('#rd_JPBO_Ch4L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch5L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_5) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_5 = Math.floor(Number($('#rd_JPBO_Ch5L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch6L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_6) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_6 = Math.floor(Number($('#rd_JPBO_Ch6L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch7L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_7) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_7 = Math.floor(Number($('#rd_JPBO_Ch7L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch8L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_8) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_8 = Math.floor(Number($('#rd_JPBO_Ch8L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch9L' , function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_9) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_9 = Math.floor(Number($('#rd_JPBO_Ch9L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch10L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_10) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_10 = Math.floor(Number($('#rd_JPBO_Ch10L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch11L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_11) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_11 = Math.floor(Number($('#rd_JPBO_Ch11L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch12L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_12) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_12 = Math.floor(Number($('#rd_JPBO_Ch12L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch13L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_13) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_13 = Math.floor(Number($('#rd_JPBO_Ch13L').val()) * 8); saveProjectAPI(); } });
        $(document).on('change', '#rd_JPBO_Ch14L', function (e) { if (verifyNumLimitsTT(e, -6, 6, Number(project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_14) / 8)) { project.systemFiles.REG_DOMAIN.JP24BOL_CHANNEL_14 = Math.floor(Number($('#rd_JPBO_Ch14L').val()) * 8); saveProjectAPI(); } });
    }
}
