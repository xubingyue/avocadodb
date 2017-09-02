/* To avoid CSS expressions while still supporting IE 7 and IE 6, use this script */
/* The script tag referring to this file must be placed before the ending body tag. */

/* Use conditional comments in order to target IE 7 and older:
	<!--[if lt IE 8]><!-->
	<script src="ie7/ie7.js"></script>
	<!--<![endif]-->
*/

(function() {
	function addIcon(el, entity) {
		var html = el.innerHTML;
		el.innerHTML = '<span style="font-family: \'avocadodb\'">' + entity + '</span>' + html;
	}
	var icons = {
		'icon_avocadodb_youtube': '&#xe600;',
		'icon_avocadodb_wireless': '&#xe601;',
		'icon_avocadodb_windows3': '&#xe602;',
		'icon_avocadodb_windows2': '&#xe603;',
		'icon_avocadodb_windows': '&#xe604;',
		'icon_avocadodb_web': '&#xe605;',
		'icon_avocadodb_warning': '&#xe606;',
		'icon_avocadodb_vimeo': '&#xe607;',
		'icon_avocadodb_unpacked': '&#xe608;',
		'icon_avocadodb_unlocked': '&#xe609;',
		'icon_avocadodb_unhappy': '&#xe60a;',
		'icon_avocadodb_twitter': '&#xe60b;',
		'icon_avocadodb_trash': '&#xe60c;',
		'icon_avocadodb_star': '&#xe60d;',
		'icon_avocadodb_stapel2': '&#xe60e;',
		'icon_avocadodb_stapel': '&#xe60f;',
		'icon_avocadodb_snapshot': '&#xe610;',
		'icon_avocadodb_shield': '&#xe611;',
		'icon_avocadodb_settings2': '&#xe612;',
		'icon_avocadodb_settings1': '&#xe613;',
		'icon_avocadodb_search': '&#xe614;',
		'icon_avocadodb_roundplus': '&#xe615;',
		'icon_avocadodb_roundminus': '&#xe616;',
		'icon_avocadodb_removefolder': '&#xe617;',
		'icon_avocadodb_removedoc': '&#xe618;',
		'icon_avocadodb_removedatabase': '&#xe619;',
		'icon_avocadodb_power': '&#xe61a;',
		'icon_avocadodb_pillow': '&#xe61b;',
		'icon_avocadodb_performance': '&#xe61c;',
		'icon_avocadodb_pencil': '&#xe61d;',
		'icon_avocadodb_packed': '&#xe61e;',
		'icon_avocadodb_note': '&#xe61f;',
		'icon_avocadodb_mark': '&#xe620;',
		'icon_avocadodb_map': '&#xe621;',
		'icon_avocadodb_mail': '&#xe622;',
		'icon_avocadodb_locked': '&#xe623;',
		'icon_avocadodb_letter': '&#xe624;',
		'icon_avocadodb_lamp': '&#xe625;',
		'icon_avocadodb_info': '&#xe626;',
		'icon_avocadodb_infinite': '&#xe627;',
		'icon_avocadodb_import': '&#xe628;',
		'icon_avocadodb_heart': '&#xe629;',
		'icon_avocadodb_happ': '&#xe62a;',
		'icon_avocadodb_gps': '&#xe62b;',
		'icon_avocadodb_google': '&#xe62c;',
		'icon_avocadodb_folder': '&#xe62d;',
		'icon_avocadodb_flag': '&#xe62e;',
		'icon_avocadodb_filter': '&#xe62f;',
		'icon_avocadodb_favorite': '&#xe630;',
		'icon_avocadodb_facebook': '&#xe631;',
		'icon_avocadodb_export': '&#xe632;',
		'icon_avocadodb_euro': '&#xe633;',
		'icon_avocadodb_edit': '&#xe634;',
		'icon_avocadodb_edge5': '&#xe635;',
		'icon_avocadodb_edge4': '&#xe636;',
		'icon_avocadodb_edge3': '&#xe637;',
		'icon_avocadodb_edge2': '&#xe638;',
		'icon_avocadodb_edge1': '&#xe639;',
		'icon_avocadodb_dotted2': '&#xe63a;',
		'icon_avocadodb_dotted': '&#xe63b;',
		'icon_avocadodb_dollar': '&#xe63c;',
		'icon_avocadodb_document2': '&#xe63d;',
		'icon_avocadodb_docs': '&#xe63e;',
		'icon_avocadodb_doc': '&#xe63f;',
		'icon_avocadodb_database1': '&#xe640;',
		'icon_avocadodb_compass': '&#xe641;',
		'icon_avocadodb_checklist': '&#xe642;',
		'icon_avocadodb_checked': '&#xe643;',
		'icon_avocadodb_chart4': '&#xe644;',
		'icon_avocadodb_chart3': '&#xe645;',
		'icon_avocadodb_chart2': '&#xe646;',
		'icon_avocadodb_chart1': '&#xe647;',
		'icon_avocadodb_bubble3': '&#xe648;',
		'icon_avocadodb_bubble2': '&#xe649;',
		'icon_avocadodb_bubble1': '&#xe64a;',
		'icon_avocadodb_box3': '&#xe64b;',
		'icon_avocadodb_box2': '&#xe64c;',
		'icon_avocadodb_box1': '&#xe64d;',
		'icon_avocadodb_books': '&#xe64e;',
		'icon_avocadodb_bookmark': '&#xe64f;',
		'icon_avocadodb_attachment': '&#xe650;',
		'icon_avocadodb_arrowright': '&#xe651;',
		'icon_avocadodb_arrowleft': '&#xe652;',
		'icon_avocadodb_arrowenter': '&#xe653;',
		'icon_avocadodb_arrow10': '&#xe654;',
		'icon_avocadodb_arrow9': '&#xe655;',
		'icon_avocadodb_arrow8': '&#xe656;',
		'icon_avocadodb_arrow7': '&#xe657;',
		'icon_avocadodb_arrow6': '&#xe658;',
		'icon_avocadodb_arrow5': '&#xe659;',
		'icon_avocadodb_arrow4': '&#xe65a;',
		'icon_avocadodb_arrow3': '&#xe65b;',
		'icon_avocadodb_arrow2': '&#xe65c;',
		'icon_avocadodb_arrow1': '&#xe65d;',
		'icon_avocadodb_addfolder': '&#xe65e;',
		'icon_avocadodb_adddoc': '&#xe65f;',
		'icon_avocadodb_adddatabase': '&#xe660;',
		'0': 0
		},
		els = document.getElementsByTagName('*'),
		i, c, el;
	for (i = 0; ; i += 1) {
		el = els[i];
		if(!el) {
			break;
		}
		c = el.className;
		c = c.match(/icon_[^\s'"]+/);
		if (c && icons[c[0]]) {
			addIcon(el, icons[c[0]]);
		}
	}
}());
