/**
 * @class Ext.form.FieldSet
 * @extends Ext.container.Container
 * 
 * A container for grouping sets of fields, rendered as a HTML `fieldset` element. The {@link #title}
 * config will be rendered as the fieldset's `legend`.
 * 
 * While FieldSets commonly contain simple groups of fields, they are general {@link Ext.container.Container Containers}
 * and may therefore contain any type of components in their {@link #items}, including other nested containers.
 * The default {@link #layout} for the FieldSet's items is `'anchor'`, but it can be configured to use any other
 * layout type.
 * 
 * FieldSets may also be collapsed if configured to do so; this can be done in two ways:
 * 
 * 1. Set the {@link #collapsible} config to true; this will result in a collapse button being rendered next to
 *    the {@link #title legend title}, or:
 * 2. Set the {@link #checkboxToggle} config to true; this is similar to using {@link #collapsible} but renders
 *    a {@link Ext.form.field.Checkbox checkbox} in place of the toggle button. The fieldset will be expanded when the
 *    checkbox is checked and collapsed when it is unchecked. The checkbox will also be included in the
 *    {@link Ext.form.Basic#submit form submit parameters} using the {@link #checkboxName} as its parameter name.
 *
 * {@img Ext.form.FieldSet/Ext.form.FieldSet.png Ext.form.FieldSet component}
 *
 * ## Example usage
 * 
 *     Ext.create('Ext.form.Panel', {
 *         title: 'Simple Form with FieldSets',
 *         labelWidth: 75, // label settings here cascade unless overridden
 *         url: 'save-form.php',
 *         frame: true,
 *         bodyStyle: 'padding:5px 5px 0',
 *         width: 550,
 *         renderTo: Ext.getBody(),
 *         layout: 'column', // arrange fieldsets side by side
 *         defaults: {
 *             bodyPadding: 4
 *         },
 *         items: [{
 *             // Fieldset in Column 1 - collapsible via toggle button
 *             xtype:'fieldset',
 *             columnWidth: 0.5,
 *             title: 'Fieldset 1',
 *             collapsible: true,
 *             defaultType: 'textfield',
 *             defaults: {anchor: '100%'},
 *             layout: 'anchor',
 *             items :[{
 *                 fieldLabel: 'Field 1',
 *                 name: 'field1'
 *             }, {
 *                 fieldLabel: 'Field 2',
 *                 name: 'field2'
 *             }]
 *         }, {
 *             // Fieldset in Column 2 - collapsible via checkbox, collapsed by default, contains a panel
 *             xtype:'fieldset',
 *             title: 'Show Panel', // title or checkboxToggle creates fieldset header
 *             columnWidth: 0.5,
 *             checkboxToggle: true,
 *             collapsed: true, // fieldset initially collapsed
 *             layout:'anchor',
 *             items :[{
 *                 xtype: 'panel',
 *                 anchor: '100%',
 *                 title: 'Panel inside a fieldset',
 *                 frame: true,
 *                 height: 52
 *             }]
 *         }]
 *     });
 * 
 * @constructor
 * Create a new FieldSet
 * @param {Object} config Configuration options
 * @xtype fieldset
 * @docauthor Jason Johnston <jason@sencha.com>
 */
Ext.define('Ext.form.FieldSet', {
    extend: 'Ext.container.Container',
    alias: 'widget.fieldset',
    uses: ['Ext.form.field.Checkbox', 'Ext.panel.Tool', 'Ext.layout.container.Anchor', 'Ext.layout.component.FieldSet'],

    /**
     * @cfg {String} title
     * A title to be displayed in the fieldset's legend. May contain HTML markup.
     */

    /**
     * @cfg {Boolean} checkboxToggle
     * Set to <tt>true</tt> to render a checkbox into the fieldset frame just
     * in front of the legend to expand/collapse the fieldset when the checkbox is toggled. (defaults
     * to <tt>false</tt>). This checkbox will be included in form submits using the {@link #checkboxName}.
     */

    /**
     * @cfg {String} checkboxName
     * The name to assign to the fieldset's checkbox if <tt>{@link #checkboxToggle} = true</tt>
     * (defaults to <tt>'[fieldset id]-checkbox'</tt>).
     */

    /**
     * @cfg {Boolean} collapsible
     * Set to <tt>true</tt> to make the fieldset collapsible and have the expand/collapse toggle button automatically
     * rendered into the legend element, <tt>false</tt> to keep the fieldset statically sized with no collapse
     * button (defaults to <tt>false</tt>). Another option is to configure <tt>{@link #checkboxToggle}</tt>.
     * Use the {@link #collapsed} config to collapse the fieldset by default.
     */

    /**
     * @cfg {Boolean} collapsed
     * Set to <tt>true</tt> to render the fieldset as collapsed by default. If {@link #checkboxToggle} is specified,
     * the checkbox will also be unchecked by default.
     */
    collapsed: false,

    /**
     * @property legend
     * @type Ext.Component
     * The component for the fieldset's legend. Will only be defined if the configuration requires a legend
     * to be created, by setting the {@link #title} or {@link #checkboxToggle} options.
     */

    /**
     * @cfg {String} baseCls The base CSS class applied to the fieldset (defaults to <tt>'x-fieldset'</tt>).
     */
    baseCls: Ext.baseCSSPrefix + 'fieldset',

    /**
     * @cfg {String} layout The {@link Ext.container.Container#layout} for the fieldset's immediate child items.
     * Defaults to <tt>'anchor'</tt>.
     */
    layout: 'anchor',

    componentLayout: 'fieldset',

    // No aria role necessary as fieldset has its own recognized semantics
    ariaRole: '',

    renderTpl: ['<div class="{baseCls}-body"></div>'],
    
    maskOnDisable: false,

    getElConfig: function(){
        return {tag: 'fieldset', id: this.id};
    },

    initComponent: function() {
        var me = this,
            baseCls = me.baseCls;

        me.callParent();

        // Create the Legend component if needed
        me.initLegend();

        // Add body el selector
        Ext.applyIf(me.renderSelectors, {
            body: '.' + baseCls + '-body'
        });

        if (me.collapsed) {
            me.addCls(baseCls + '-collapsed');
            me.collapse();
        }
    },

    // private
    onRender: function(container, position) {
        this.callParent(arguments);
        // Make sure the legend is created and rendered
        this.initLegend();
    },

    /**
     * @private
     * Initialize and render the legend component if necessary
     */
    initLegend: function() {
        var me = this,
            legendItems,
            legend = me.legend;

        // Create the legend component if needed and it hasn't been already
        if (!legend && (me.title || me.checkboxToggle || me.collapsible)) {
            legendItems = [];

            // Checkbox
            if (me.checkboxToggle) {
                legendItems.push(me.createCheckboxCmp());
            }
            // Toggle button
            else if (me.collapsible) {
                legendItems.push(me.createToggleCmp());
            }

            // Title
            legendItems.push(me.createTitleCmp());

            legend = me.legend = Ext.create('Ext.container.Container', {
                baseCls: me.baseCls + '-header',
                ariaRole: '',
                getElConfig: function(){
                    return {tag: 'legend', cls: this.baseCls};
                },
                items: legendItems
            });
        }

        // Make sure legend is rendered if the fieldset is rendered
        if (legend && !legend.rendered && me.rendered) {
            me.legend.render(me.el, me.body); //insert before body element
        }
    },

    /**
     * @protected
     * Creates the legend title component. This is only called internally, but could be overridden in subclasses
     * to customize the title component.
     * @return Ext.Component
     */
    createTitleCmp: function() {
        var me = this;
        me.titleCmp = Ext.create('Ext.Component', {
            html: me.title,
            cls: me.baseCls + '-header-text'
        });
        return me.titleCmp;
        
    },

    /**
     * @property checkboxCmp
     * @type Ext.form.field.Checkbox
     * Refers to the {@link Ext.form.field.Checkbox} component that is added next to the title in the legend. Only
     * populated if the fieldset is configured with <tt>{@link #checkboxToggle}:true</tt>.
     */

    /**
     * @protected
     * Creates the checkbox component. This is only called internally, but could be overridden in subclasses
     * to customize the checkbox's configuration or even return an entirely different component type.
     * @return Ext.Component
     */
    createCheckboxCmp: function() {
        var me = this,
            suffix = '-checkbox';
            
        me.checkboxCmp = Ext.create('Ext.form.field.Checkbox', {
            name: me.checkboxName || me.id + suffix,
            cls: me.baseCls + '-header' + suffix,
            checked: !me.collapsed,
            listeners: {
                change: me.onCheckChange,
                scope: me
            }
        });
        return me.checkboxCmp;
    },

    /**
     * @property toggleCmp
     * @type Ext.panel.Tool
     * Refers to the {@link Ext.panel.Tool} component that is added as the collapse/expand button next
     * to the title in the legend. Only populated if the fieldset is configured with <tt>{@link #collapsible}:true</tt>.
     */

    /**
     * @protected
     * Creates the toggle button component. This is only called internally, but could be overridden in
     * subclasses to customize the toggle component.
     * @return Ext.Component
     */
    createToggleCmp: function() {
        var me = this;
        me.toggleCmp = Ext.create('Ext.panel.Tool', {
            type: 'toggle',
            handler: me.toggle,
            scope: me
        });
        return me.toggleCmp;
    },
    
    /**
     * Sets the title of this fieldset
     * @param {String} title The new title
     * @return {Ext.form.FieldSet} this
     */
    setTitle: function(title) {
        var me = this;
        me.title = title;
        me.initLegend();
        me.titleCmp.update(title);
        return me;
    },
    
    getTargetEl : function() {
        return this.body || this.frameBody || this.el;
    },
    
    getContentTarget: function() {
        return this.body;
    },
    
    /**
     * @private
     * Include the legend component in the items for ComponentQuery
     */
    getRefItems: function(deep) {
        var refItems = this.callParent(arguments),
            legend = this.legend;

        // Prepend legend items to ensure correct order
        if (legend) {
            refItems.unshift(legend);
            if (deep) {
                refItems.unshift.apply(refItems, legend.getRefItems(true));
            }
        }
        return refItems;
    },

    /**
     * Expands the fieldset.
     * @return {Ext.form.FieldSet} this
     */
    expand : function(){
        return this.setExpanded(true);
    },
    
    /**
     * Collapses the fieldset.
     * @return {Ext.form.FieldSet} this
     */
    collapse : function() {
        return this.setExpanded(false);
    },

    /**
     * @private Collapse or expand the fieldset
     */
    setExpanded: function(expanded) {
        var me = this,
            checkboxCmp = me.checkboxCmp;

        expanded = !!expanded;
        
        if (checkboxCmp) {
            checkboxCmp.setValue(expanded);
        }
        
        if (expanded) {
            me.removeCls(me.baseCls + '-collapsed');
        } else {
            me.addCls(me.baseCls + '-collapsed');
        }
        me.collapsed = !expanded;
        if (expanded) {
            // ensure subitems will get rendered and layed out when expanding
            me.getComponentLayout().childrenChanged = true;
        }
        me.doComponentLayout();
        return me;
    },

    /**
     * Toggle the fieldset's collapsed state to the opposite of what it is currently
     */
    toggle: function() {
        this.setExpanded(!!this.collapsed);
    },

    /**
     * @private Handle changes in the checkbox checked state
     */
    onCheckChange: function(cmp, checked) {
        this.setExpanded(checked);
    },

    beforeDestroy : function() {
        var legend = this.legend;
        if (legend) {
            legend.destroy();
        }
        this.callParent();
    }
});
