import {createStore} from 'vuex';
import AuthModule from './modules/auth';
import ShelvesModule from './modules/shelves';
import TripsModule from './modules/trips';
import CategoriesModule from './modules/categories';

const store = createStore({
    modules:{
        auth:AuthModule,
        shelves:ShelvesModule,
        trips:TripsModule,
        categories:CategoriesModule
    }
});

export default store;