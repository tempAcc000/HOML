/**
 * generated by Xtext 2.26.0
 */
package org.consensus.homodel.hOModel;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Statement</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.consensus.homodel.hOModel.Statement#getVar <em>Var</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.Statement#getExp <em>Exp</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.Statement#getStatement <em>Statement</em>}</li>
 * </ul>
 *
 * @see org.consensus.homodel.hOModel.HOModelPackage#getStatement()
 * @model
 * @generated
 */
public interface Statement extends EObject
{
  /**
   * Returns the value of the '<em><b>Var</b></em>' containment reference list.
   * The list contents are of type {@link org.consensus.homodel.hOModel.Variable}.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the value of the '<em>Var</em>' containment reference list.
   * @see org.consensus.homodel.hOModel.HOModelPackage#getStatement_Var()
   * @model containment="true"
   * @generated
   */
  EList<Variable> getVar();

  /**
   * Returns the value of the '<em><b>Exp</b></em>' containment reference list.
   * The list contents are of type {@link org.eclipse.emf.ecore.EObject}.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the value of the '<em>Exp</em>' containment reference list.
   * @see org.consensus.homodel.hOModel.HOModelPackage#getStatement_Exp()
   * @model containment="true"
   * @generated
   */
  EList<EObject> getExp();

  /**
   * Returns the value of the '<em><b>Statement</b></em>' containment reference list.
   * The list contents are of type {@link org.consensus.homodel.hOModel.Statement}.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the value of the '<em>Statement</em>' containment reference list.
   * @see org.consensus.homodel.hOModel.HOModelPackage#getStatement_Statement()
   * @model containment="true"
   * @generated
   */
  EList<Statement> getStatement();

} // Statement
