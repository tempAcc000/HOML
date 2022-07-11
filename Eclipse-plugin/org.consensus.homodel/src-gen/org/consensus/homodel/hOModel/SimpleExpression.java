/**
 * generated by Xtext 2.26.0
 */
package org.consensus.homodel.hOModel;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Simple Expression</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.consensus.homodel.hOModel.SimpleExpression#getTerm <em>Term</em>}</li>
 * </ul>
 *
 * @see org.consensus.homodel.hOModel.HOModelPackage#getSimpleExpression()
 * @model
 * @generated
 */
public interface SimpleExpression extends EObject
{
  /**
   * Returns the value of the '<em><b>Term</b></em>' containment reference list.
   * The list contents are of type {@link org.consensus.homodel.hOModel.Term}.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the value of the '<em>Term</em>' containment reference list.
   * @see org.consensus.homodel.hOModel.HOModelPackage#getSimpleExpression_Term()
   * @model containment="true"
   * @generated
   */
  EList<Term> getTerm();

} // SimpleExpression