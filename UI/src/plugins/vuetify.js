import Vue from 'vue';
import Vuetify from 'vuetify/lib';
import colors from 'vuetify/lib/util/colors'

Vue.use(Vuetify);
Vue.directive('visible', function(el, binding) {
    el.prevDisplay = el.style.display;
	el.style.display = binding.value ? '' : 'none';
});

export default new Vuetify({
	icons: {
		iconfont: 'mdi',
	},
	theme: {
		themes: {
			dark: {
				primary: colors.coal,
				secondary: colors.teal,
				accent: colors.shades.dark,
				error: colors.red.accent3
			}
		}
	}
})