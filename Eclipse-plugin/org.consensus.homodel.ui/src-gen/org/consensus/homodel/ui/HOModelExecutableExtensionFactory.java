/*
 * generated by Xtext 2.26.0
 */
package org.consensus.homodel.ui;

import com.google.inject.Injector;
import org.consensus.homodel.ui.internal.HomodelActivator;
import org.eclipse.xtext.ui.guice.AbstractGuiceAwareExecutableExtensionFactory;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;

/**
 * This class was generated. Customizations should only happen in a newly
 * introduced subclass. 
 */
public class HOModelExecutableExtensionFactory extends AbstractGuiceAwareExecutableExtensionFactory {

	@Override
	protected Bundle getBundle() {
		return FrameworkUtil.getBundle(HomodelActivator.class);
	}
	
	@Override
	protected Injector getInjector() {
		HomodelActivator activator = HomodelActivator.getInstance();
		return activator != null ? activator.getInjector(HomodelActivator.ORG_CONSENSUS_HOMODEL_HOMODEL) : null;
	}

}